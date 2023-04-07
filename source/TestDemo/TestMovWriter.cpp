#include <cstdio>
#include <cstring>
#include <cmath>

extern "C" 
{
#include "libavutil/error.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/codec_id.h"
}

int test_mov_writer()
{
	int nRet = 0;
	AVFormatContext *pFormatCtx = nullptr;
	const char *filename = "/home/cent/media/test.mov";
	int nWidth = 3840;
	int nHeight = 2160;
	int nBitRate = 8000000;
	int nFrames = 0;
	int llDuration = 200; //50P
	AVPacket pkt;
	FILE* fpStreamReader;
	const AVCodec *avcodec = nullptr;
	AVStream *out_stream = nullptr;
	AVCodecParameters *codecpar = nullptr;

	int bitStreamLen = 3680000;
	unsigned char* streamBuf = new unsigned char[bitStreamLen];

	fpStreamReader = fopen("/home/cent/media/4k_hq.bin", "rb");
	if (!fpStreamReader)
	{
		printf("open bitstream file error\n");
		return 0;
	}

	fread(streamBuf, bitStreamLen, 1, fpStreamReader);

	nRet = avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, filename);
	if (nRet < 0 || !pFormatCtx)
	{
		printf("avformat_alloc_output_context2 error\n");
		goto error;
	}

	out_stream = avformat_new_stream(pFormatCtx, nullptr);
	codecpar = out_stream->codecpar;
	codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	codecpar->codec_id = AV_CODEC_ID_PRORES;
	codecpar->width = nWidth;
	codecpar->height = nHeight;
	codecpar->bit_rate = nBitRate;
	out_stream->time_base.num = 1;
	out_stream->time_base.den = 10000;
	codecpar->bits_per_raw_sample = 8;//10bit视频填10
	out_stream->avg_frame_rate.num = 50; //帧率
	out_stream->avg_frame_rate.den = 1;

	//VX_TYPE_VIDEO_PRORES_STANDARD:
	codecpar->codec_tag = MKTAG('a', 'p', 'c', 'n');
	//VX_TYPE_VIDEO_PRORES_PROXY:
	//codecpar->codec_tag = MKTAG('a', 'p', 'c', 'o');
	//VX_TYPE_VIDEO_PRORES_LT:
	//codecpar->codec_tag = MKTAG('a', 'p', 'c', 's');
	//VX_TYPE_VIDEO_PRORES_HQ:
	//codecpar->codec_tag = MKTAG('a', 'p', 'c', 'h');
	//VX_TYPE_VIDEO_PRORES_4444:
	//codecpar->codec_tag = MKTAG('a', 'p', '4', 'h');
	//VX_TYPE_VIDEO_PRORES_4444XQ:
	//codecpar->codec_tag = MKTAG('a', 'p', '4', 'x');

	codecpar->format = AV_PIX_FMT_YUV420P;

	av_dump_format(pFormatCtx, 0, filename, 1);

	/* open the output file, if needed */
	if (!(pFormatCtx->oformat->flags & AVFMT_NOFILE)) {
		nRet = avio_open(&pFormatCtx->pb, filename, AVIO_FLAG_WRITE);
		if (nRet < 0) 
		{
			printf("Could not open '%s'\n", filename);
			goto error;
		}
	}

	nRet = avformat_write_header(pFormatCtx, nullptr);
	if (nRet < 0)
	{
		printf("avformat_write_header error\n");
		goto error;
	}

	//送帧
	for (int i = 0; i < 100; i++)
	{
		printf("writing frame %d\n", i);

		nFrames++;

		memset(&pkt, 0, sizeof(pkt));
		av_init_packet(&pkt);
		av_grow_packet(&pkt, bitStreamLen - pkt.size);
		memcpy(pkt.data, streamBuf, pkt.size);
		pkt.stream_index = 0;
		pkt.pts = pkt.dts = nFrames * llDuration;
		pkt.duration = llDuration;

		AVRational timeSq;
		AVStream *pStream = pFormatCtx->streams[0];

		int nRet = av_interleaved_write_frame(pFormatCtx, &pkt);
		if (nRet < 0)
		{
			printf("av_interleaved_write_frame error\n");
			goto error;
		}

	}

	nRet = av_write_trailer(pFormatCtx);
	if (nRet < 0)
	{
		printf("av_write_trailer error\n");
		goto error;
	}

error:
	if (pFormatCtx)
	{
		avformat_free_context(pFormatCtx);
		pFormatCtx = nullptr;
	}

	if (fpStreamReader)
	{
		fclose(fpStreamReader);
		fpStreamReader = nullptr;
	}

	if (streamBuf)
	{
		delete[] streamBuf;
		streamBuf = nullptr;
	}

	return 0;
}