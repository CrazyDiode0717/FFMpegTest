extern "C"
{
#include "libavformat/avformat.h"
}

#include "AvioImpl.h"

#include <memory>
#include <functional>
#include <fstream>

struct stAvFormatScopedNetwork
{
    stAvFormatScopedNetwork()
    {
        avformat_network_init();
    }

    ~stAvFormatScopedNetwork()
    {
        avformat_network_deinit();
    }
};

typedef std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> AVPacketPtr;

inline AVPacketPtr AllocAVPacket()
{
    return AVPacketPtr(av_packet_alloc(),
                       [](AVPacket* pObject)
                       {
                           av_packet_free(&pObject);
                       });
}

void AnalyzePacket(AVFormatContext* pFmtCtx, AVPacket* pPkt)
{
    switch(pFmtCtx->streams[pPkt->stream_index]->codecpar->codec_type)
    {
        case AVMEDIA_TYPE_VIDEO:
        printf("Video packet\n");
        break;
        case AVMEDIA_TYPE_AUDIO:
        printf("Audio packet\n");
        break;
        case AVMEDIA_TYPE_DATA:
        printf("Data packet\n");
        break;
        case AVMEDIA_TYPE_SUBTITLE:
        printf("Subtitle packet\n");
        break;
        case AVMEDIA_TYPE_ATTACHMENT:
        printf("Attachment packet\n");
        break;
        default:
        printf("Invalid packet\n");
        break;
    }

    av_pkt_dump_log2(NULL, AV_LOG_INFO, pPkt, 0, pFmtCtx->streams[pPkt->stream_index]);
}

int TestDemux()
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
        return nRet;
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
        return nRet;
    }

    AVDurationEstimationMethod dm = av_fmt_ctx_get_duration_estimation_method(ptrFmtCtx.get());

    AVPacketPtr ptrPkt = AllocAVPacket();

    nRet = av_read_frame(ptrFmtCtx.get(), ptrPkt.get());
    if(nRet < 0)
    {
        printf("Read frame failed\n");
        return nRet;
    }
    AnalyzePacket(ptrFmtCtx.get(), ptrPkt.get());

    int videoStream = av_find_best_stream(ptrFmtCtx.get(), AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream* pVideoStream = ptrFmtCtx->streams[videoStream];
    const AVIndexEntry* pEntry = nullptr;
    
    if(ptrFmtCtx->pb->seekable)
    {
        double timeScale = av_q2d(av_inv_q(pVideoStream->time_base));
        double frameDuration = av_q2d(av_inv_q(pVideoStream->r_frame_rate));
        int64_t timestamp = 100 * timeScale  * frameDuration;
        nRet = avformat_seek_file(ptrFmtCtx.get(), videoStream, INT64_MIN, timestamp, INT64_MAX, AVSEEK_FLAG_BACKWARD);

        pEntry = avformat_index_get_entry_from_timestamp(pVideoStream, timestamp, AVSEEK_FLAG_BACKWARD);
    }

    ptrPkt = AllocAVPacket();

    nRet = av_read_frame(ptrFmtCtx.get(), ptrPkt.get());
    if(nRet < 0)
    {
        printf("Read frame failed\n");
        return nRet;
    }
    AnalyzePacket(ptrFmtCtx.get(), ptrPkt.get());


    int nIndexEntryCount = avformat_index_get_entries_count(pVideoStream);
    const AVIndexEntry* pIndexEntry = avformat_index_get_entry(pVideoStream, 0);


#if 0
    std::ifstream fileReader("/home/cent/media/src1.mp4", std::ios::binary);
    std::unique_ptr<uint8_t[]> ptrBuf(new uint8_t[1024]);
    fileReader.read((char*)ptrBuf.get(), 1024);
    fileReader.close();

    AVProbeData pd{0};
    pd.buf = ptrBuf.get();
    pd.buf_size = 1024;
    pd.filename = "/home/cent/media/src1.mp4";

    int score = 0;
    const AVInputFormat* ifmt = av_probe_input_format3(&pd, 1, &score);
#endif
}

#define USE_USER_IO 0

int TestMux()
{
 	int nRet = 0;
	int nTotalFrames = 0;
	int nPktDuration = 1; //25P
	const char* filename = "/home/cent/media/test.mxf";

	int videoBitStreamLen = 1260032;
	std::unique_ptr<uint8_t[]> ptrVideoBuf(new uint8_t[videoBitStreamLen]);
	std::ifstream fileReader("/home/cent/media/ffmpeg_xavc.h264", std::ios::binary);
	fileReader.read(reinterpret_cast<char*>(ptrVideoBuf.get()), videoBitStreamLen);
	fileReader.close();

	int audioBitStreamLen = 3840;
	std::unique_ptr<uint8_t[]> ptrAudioBuf(new uint8_t[audioBitStreamLen]);

	av_log_set_level(AV_LOG_DEBUG);


	AVFormatContext* pFormatCtx = nullptr;
#if USE_USER_IO
	//重载IO就不传文件名了
	nRet = avformat_alloc_output_context2(&pFormatCtx, nullptr, "mxf", nullptr);
#else
	nRet = avformat_alloc_output_context2(&pFormatCtx, nullptr, nullptr, filename);
#endif
    
	if (nRet < 0 || !pFormatCtx)
	{
		printf("avformat_alloc_output_context2 error\n");
		return 0;
	}

	std::unique_ptr < AVFormatContext, std::function<void(AVFormatContext*)>> ptrFormatCtx(pFormatCtx, [](AVFormatContext* p) {
		if (p)
		{
#if USE_USER_IO
#else
			/* close output */
			if (p && !(p->flags & AVFMT_NOFILE))
				avio_closep(&p->pb);
			avformat_free_context(p);
#endif
		}
		});

	//add video stream
	AVStream* videoStream = avformat_new_stream(ptrFormatCtx.get(), nullptr);
	videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	videoStream->codecpar->codec_id = AV_CODEC_ID_H264;
	videoStream->codecpar->width = 3840;
	videoStream->codecpar->height = 2160;
	videoStream->codecpar->bit_rate = 5000000; //码率500M
	videoStream->codecpar->bits_per_raw_sample = 10;//10bit视频填10
	videoStream->codecpar->format = AV_PIX_FMT_YUV422P10LE;
	videoStream->avg_frame_rate.num = 25; //帧率
	videoStream->avg_frame_rate.den = 1;
	videoStream->time_base.num = 1;
	videoStream->time_base.den = 25;

	AVStream* audioStream = avformat_new_stream(ptrFormatCtx.get(), nullptr);
	audioStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	audioStream->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;
	audioStream->codecpar->format = AV_SAMPLE_FMT_S16;
	audioStream->codecpar->bits_per_raw_sample = 16;
	audioStream->codecpar->bits_per_coded_sample = 16;
	audioStream->codecpar->block_align = 16;
	audioStream->codecpar->sample_rate = 48000;
	audioStream->codecpar->channels = 1;
	audioStream->codecpar->frame_size = audioBitStreamLen;
	audioStream->time_base.num = 1;
	audioStream->time_base.den = 48000;

	av_dump_format(ptrFormatCtx.get(), 0, filename, 1);

#if USE_USER_IO
	CVXAvIO fileIO;
	fileIO.SetFileName(filename, true);

	ptrFormatCtx->pb = fileIO.GetAVIO();
#else
	/* open the output file, if needed */
	if (!(ptrFormatCtx->oformat->flags & AVFMT_NOFILE)) 
    {
		nRet = avio_open(&ptrFormatCtx->pb, filename, AVIO_FLAG_WRITE);
		if (nRet < 0) 
		{
			printf("Could not open '%s'\n", filename);
			return 0;
		}
	}
#endif

	nRet = av_dict_set(&ptrFormatCtx->metadata, "test_dict", "test", 0);
	if (nRet < 0)
	{
		printf("set dict error\n");
		return 0;
	}

	nRet = avformat_write_header(ptrFormatCtx.get(), nullptr);
	if (nRet < 0)
	{
		printf("avformat_write_header error\n");
		return 0;
	}

	//送帧
	for (int i = 0; i < 100; i++)
	{
		printf("writing frame %d\n", i);

		nTotalFrames++;
		
		//video
        AVPacketPtr ptrVideoPkt = AllocAVPacket();
		av_new_packet(ptrVideoPkt.get(), videoBitStreamLen);
        memcpy(ptrVideoPkt->data, ptrVideoBuf.get(), ptrVideoPkt->size);
		ptrVideoPkt->stream_index = videoStream->index;
		ptrVideoPkt->pts = ptrVideoPkt->dts = nTotalFrames * nPktDuration;
		ptrVideoPkt->duration = nPktDuration;

		int nRet = av_interleaved_write_frame(ptrFormatCtx.get(), ptrVideoPkt.get());
		if (nRet < 0)
		{
			printf("write video packet error\n");
			return 0;
		}

		//audio
        AVPacketPtr ptrAudioPkt = AllocAVPacket();
		av_new_packet(ptrAudioPkt.get(), audioBitStreamLen);
        memcpy(ptrAudioPkt->data, ptrAudioBuf.get(), ptrAudioPkt->size);
		ptrAudioPkt->stream_index = audioStream->index;
		ptrAudioPkt->pts = ptrAudioPkt->dts = nTotalFrames * 1920;
		ptrAudioPkt->duration = 1920;

		nRet = av_interleaved_write_frame(ptrFormatCtx.get(), ptrAudioPkt.get());
		if (nRet < 0)
		{
			printf("write audio packet error\n");
			return 0;
		}
	}

	nRet = av_write_trailer(ptrFormatCtx.get());
	if (nRet < 0)
	{
		printf("av_write_trailer error\n");
		return 0;
	}

	return 0;
}

void TestAvformat()
{
    stAvFormatScopedNetwork scopedNetwork;

    //TestDemux();
    TestMux();

}