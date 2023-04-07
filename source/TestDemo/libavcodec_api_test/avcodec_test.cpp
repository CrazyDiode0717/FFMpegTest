extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/cpu.h>
}

#include <memory>
#include <functional>
#include <fstream>
#include <algorithm>

typedef std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> AVPacketPtr;

inline AVPacketPtr AllocAVPacket()
{
    return AVPacketPtr(av_packet_alloc(),
                       [](AVPacket* pObject)
                       {
                           av_packet_free(&pObject);
                       });
}

typedef std::unique_ptr<AVFrame, std::function<void(AVFrame*)>> AVFramePtr;

inline AVFramePtr AllocAVFrame()
{
    return AVFramePtr(av_frame_alloc(),
                      [](AVFrame* pObject)
                      {
                          av_frame_free(&pObject);
                      });
}

int TestEncodeVideo()
{
    int nRet = 0;
    AVCodecContext* pEncoderContext = avcodec_alloc_context3(avcodec_find_encoder(AV_CODEC_ID_H264));

    pEncoderContext->width = 1920;
    pEncoderContext->height = 1080;
    pEncoderContext->time_base = { 1, 25 };
    pEncoderContext->framerate = { 25, 1 };
    pEncoderContext->bit_rate = 2000000;
    pEncoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
    pEncoderContext->thread_count = std::min(av_cpu_count(), 8);
    pEncoderContext->gop_size = 25;
    pEncoderContext->max_b_frames = 0;

    AVDictionary* pEncoderOptions = nullptr;
    av_dict_set(&pEncoderOptions, "preset", "ultrafast", 0);
    av_dict_set(&pEncoderOptions, "tune", "zerolatency", 0);

    nRet = avcodec_open2(pEncoderContext, nullptr, &pEncoderOptions);

    if (pEncoderOptions)
    {
        av_dict_free(&pEncoderOptions);
        pEncoderOptions = nullptr;
    }

    if (nRet < 0)
    {
        if (pEncoderContext)
        {
            avcodec_free_context(&pEncoderContext);
            pEncoderContext = nullptr;
        }
        return nRet;
    };
    AVPacketPtr  ptrPkt  = AllocAVPacket();
    AVFramePtr ptrFrame = AllocAVFrame();
    ptrFrame->width = pEncoderContext->width;
    ptrFrame->height = pEncoderContext->height;
    ptrFrame->format = pEncoderContext->pix_fmt;

    int imageSize = av_image_get_buffer_size(pEncoderContext->pix_fmt, pEncoderContext->width, pEncoderContext->height, 1);
    std::unique_ptr<uint8_t[]> ptrImgBuf(new uint8_t[imageSize]);
    nRet = av_image_fill_arrays(ptrFrame->data, ptrFrame->linesize, ptrImgBuf.get(), (AVPixelFormat)ptrFrame->format, ptrFrame->width, ptrFrame->height, 1);

    std::ifstream yuvReader("/home/cent/media/1920x1080.yuv", std::ios::binary);
    std::ofstream bitStreamWriter("/home/cent/media/encode.h264", std::ios::binary);

    for(int i = 0;  i < 100; i++)
    {
        yuvReader.read((char*)ptrImgBuf.get(), imageSize);
        ptrFrame->pts = i;
        nRet = avcodec_send_frame(pEncoderContext, ptrFrame.get());
        while(nRet >= 0)
        {
            nRet = avcodec_receive_packet(pEncoderContext, ptrPkt.get());
            if(nRet >= 0)
            {
                bitStreamWriter.write((char*)ptrPkt->data, ptrPkt->size);
                av_packet_unref(ptrPkt.get());
            }
        }
        
    }

    nRet = avcodec_send_frame(pEncoderContext, nullptr);
    while (nRet >= 0)
    {
        nRet = avcodec_receive_packet(pEncoderContext, ptrPkt.get());
        if (nRet >= 0)
        {
            bitStreamWriter.write((char*)ptrPkt->data, ptrPkt->size);
            av_packet_unref(ptrPkt.get());
        }
    }

    if (pEncoderContext)
    {
        avcodec_free_context(&pEncoderContext);
        pEncoderContext = nullptr;
    }

    yuvReader.close();
    bitStreamWriter.close();

    return 0;
}

int TestDecodeVideo()
{
    int nRet = 0;
    AVCodecContext* pDecoderContext = avcodec_alloc_context3(avcodec_find_decoder(AV_CODEC_ID_H264));

    pDecoderContext->width = 1920;
    pDecoderContext->height = 1080;
    pDecoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
    pDecoderContext->thread_count = 8;

    nRet = avcodec_open2(pDecoderContext, nullptr, nullptr);
    if (nRet < 0)
    {
        if (pDecoderContext)
        {
            avcodec_free_context(&pDecoderContext);
            pDecoderContext = nullptr;
        }

        return nRet;
    }

    AVPacketPtr  ptrPkt  = AllocAVPacket();
    AVFramePtr ptrFrame = AllocAVFrame();

    std::ifstream bitStreamReader("/home/cent/media/ffmpeg_xavc.h264", std::ios::binary);
    std::ofstream yuvWriter("/home/cent/media/xavc_dec.yuv", std::ios::binary);
    int nBitStreamLen = 1260032;
    std::unique_ptr<uint8_t[]> ptrBuf(new uint8_t[nBitStreamLen]);
    bitStreamReader.read((char*)ptrBuf.get(), nBitStreamLen);

    ptrPkt->data = ptrBuf.get();
    ptrPkt->size = nBitStreamLen;
    ptrPkt->pts = 0;
    ptrPkt->dts = 0;

    nRet = avcodec_send_packet(pDecoderContext, ptrPkt.get());
    while(nRet >= 0)
    {
        nRet = avcodec_receive_frame(pDecoderContext, ptrFrame.get());

        if (nRet >= 0)
        {
            yuvWriter.write((char*)ptrFrame->data[0], ptrFrame->linesize[0] * ptrFrame->height);
            yuvWriter.write((char*)ptrFrame->data[1], ptrFrame->linesize[1] * ptrFrame->height);
            yuvWriter.write((char*)ptrFrame->data[2], ptrFrame->linesize[2] * ptrFrame->height);
            av_frame_unref(ptrFrame.get());
        }
    }

    nRet = avcodec_send_packet(pDecoderContext, nullptr);
    while(nRet >= 0)
    {
        nRet = avcodec_receive_frame(pDecoderContext, ptrFrame.get());

        if (nRet >= 0)
        {
            yuvWriter.write((char*)ptrFrame->data[0], ptrFrame->linesize[0] * ptrFrame->height);
            yuvWriter.write((char*)ptrFrame->data[1], ptrFrame->linesize[1] * ptrFrame->height);
            yuvWriter.write((char*)ptrFrame->data[2], ptrFrame->linesize[2] * ptrFrame->height);
            av_frame_unref(ptrFrame.get());
        }
    }

    if (pDecoderContext)
    {
        avcodec_free_context(&pDecoderContext);
        pDecoderContext = nullptr;
    }

    bitStreamReader.close();
    yuvWriter.close();

    return 0;
}

int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) 
    {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

int TestEncodeAudio()
{
    int nRet = 0;
    AVCodecContext* pEncoderContext = avcodec_alloc_context3(avcodec_find_encoder(AV_CODEC_ID_MP2));

    pEncoderContext->time_base = { 1, 48000 };
    pEncoderContext->bit_rate = 64000;
    pEncoderContext->sample_fmt = AV_SAMPLE_FMT_S16;
    pEncoderContext->sample_rate = 48000;
    pEncoderContext->ch_layout = AV_CHANNEL_LAYOUT_STEREO;

    if (!check_sample_fmt(pEncoderContext->codec, pEncoderContext->sample_fmt)) 
    {
        printf("Encoder does not support sample format %s", av_get_sample_fmt_name(pEncoderContext->sample_fmt));
        return 0;
    }

    nRet = avcodec_open2(pEncoderContext, nullptr, nullptr);
    if (nRet < 0)
    {
        if (pEncoderContext)
        {
            avcodec_free_context(&pEncoderContext);
            pEncoderContext = nullptr;
        }
        return nRet;
    };

    AVPacketPtr  ptrPkt  = AllocAVPacket();
    AVFramePtr ptrFrame = AllocAVFrame();
    ptrFrame->nb_samples = pEncoderContext->frame_size;
    ptrFrame->format = pEncoderContext->sample_fmt;
    av_channel_layout_copy(&ptrFrame->ch_layout, &pEncoderContext->ch_layout);

    nRet = av_frame_get_buffer(ptrFrame.get(), 0);
    if (nRet < 0) 
    {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        return 0;
    }

    std::ofstream bitStreamWriter("/home/cent/media/encode.mp2", std::ios::binary);

    float t = 0, tincr = 0;
    uint16_t *samples = nullptr;

    tincr = 2 * M_PI * 440.0 / pEncoderContext->sample_rate;
    for (int i = 0; i < 200; i++) 
    {
        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        nRet = av_frame_make_writable(ptrFrame.get());
        if (nRet < 0)
        {
            printf("make writable failed\n");
            return nRet;
        }

        samples = (uint16_t*)ptrFrame->data[0];

        for (int j = 0; j < pEncoderContext->frame_size; j++) 
        {
            samples[2*j] = (int)(sin(t) * 10000);

            for (int k = 1; k < pEncoderContext->ch_layout.nb_channels; k++)
                samples[2*j + k] = samples[2*j];
            
            t += tincr;
        }

        nRet = avcodec_send_frame(pEncoderContext, ptrFrame.get());
        while (nRet >= 0)
        {
            nRet = avcodec_receive_packet(pEncoderContext, ptrPkt.get());
            if (nRet >= 0)
            {
                bitStreamWriter.write((char*)ptrPkt->data, ptrPkt->size);
                av_packet_unref(ptrPkt.get());
            }
        }
    }

    nRet = avcodec_send_frame(pEncoderContext, nullptr);
    while (nRet >= 0)
    {
        nRet = avcodec_receive_packet(pEncoderContext, ptrPkt.get());
        if (nRet >= 0)
        {
            bitStreamWriter.write((char*)ptrPkt->data, ptrPkt->size);
            av_packet_unref(ptrPkt.get());
        }
    }

    if (pEncoderContext)
    {
        avcodec_free_context(&pEncoderContext);
        pEncoderContext = nullptr;
    }

    bitStreamWriter.close();
    return 0;
}

void TestAvcodec()
{
    //TestEncodeVideo();

    //TestDecodeVideo();

    TestEncodeAudio();
}