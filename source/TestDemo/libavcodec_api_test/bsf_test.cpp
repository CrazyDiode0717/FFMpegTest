extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/bsf.h"
}

#include <memory>
#include <functional>

typedef std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> AVPacketPtr;

inline AVPacketPtr AllocAVPacket()
{
    return AVPacketPtr(av_packet_alloc(),
                       [](AVPacket* pObject)
                       {
                           av_packet_free(&pObject);
                       });
}

void TestBsf()
{
    int nRet = 0;
    const char* url = "/home/cent/media/src1.mp4";

    AVDictionary* pOptions = nullptr;
    //av_dict_set(&pOptions, "protocol_blacklist", "file,http,https,rtp,udp,tcp,tls", 0);
    
    AVFormatContext* pFmtCtx = nullptr;
    nRet = avformat_open_input(&pFmtCtx, url, nullptr, &pOptions);
    if (pOptions != nullptr)
    {
        av_dict_free(&pOptions);
    }
    if (nRet < 0)
    {
        //avformat_open_input失败内部会调avformat_free_context
        printf("Could not open source file %s\n", url);
        return;
    }

     std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext*)>> ptrFmtCtx(pFmtCtx, [](AVFormatContext* p) {
        if (p)
        {
            avformat_close_input(&p);
        }
        });
    
    for(int i = 0; i < ptrFmtCtx->nb_streams; i++)
    {
        AVStream* pStream = ptrFmtCtx->streams[i];
        if(pStream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            pStream->discard = AVDISCARD_ALL;
    }


    nRet = avformat_find_stream_info(ptrFmtCtx.get(), nullptr);
    if (nRet < 0)
    {
        printf("Could not find stream information\n");
        return;
    }

    /*init bitstream filter*/
    AVBSFContext* pBsfContext = nullptr;
    nRet = av_bsf_alloc(av_bsf_get_by_name("h264_mp4toannexb"), &pBsfContext);
    if(nRet < 0)
    {
        printf("alloc bsf failed\n");
        return;
    }
    
    pBsfContext->time_base_in = ptrFmtCtx->streams[0]->time_base;

    nRet = avcodec_parameters_copy(pBsfContext->par_in, ptrFmtCtx->streams[0]->codecpar);
    if (nRet < 0)
    {
        printf("copy codecpar\n");
        return;
    }

    nRet = av_bsf_init(pBsfContext);
    if(nRet <0)
    {
        printf("init bsf failed\n");
        return;
    }


    while (nRet >= 0)
    {
        AVPacketPtr ptrPkt = AllocAVPacket();
        nRet = av_read_frame(ptrFmtCtx.get(), ptrPkt.get());
        if (nRet < 0)
        {
            printf("Read frame failed\n");
            return;
        }

        if(ptrFmtCtx->streams[ptrPkt->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            av_bsf_send_packet(pBsfContext, ptrPkt.get());

            AVPacketPtr ptrPkt1 = AllocAVPacket();
            av_bsf_receive_packet(pBsfContext, ptrPkt1.get());
        }
    }

    if (pBsfContext != nullptr)
    {
        av_bsf_flush(pBsfContext);
        av_bsf_free(&pBsfContext);
        pBsfContext = nullptr;
    }
}