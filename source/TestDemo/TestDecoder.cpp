#include "FFDemuxer.h"
#include "FFDecoder.h"
#include "FFUtils.h"
#include "FFScale.h"
#include "FFResample.h"
#include <fstream>

std::ofstream rgbWriter;
std::ofstream pcmWriter;

FFScale* pVideoScale = nullptr;
FFResample* pAudioResampler = nullptr;

int VideoDecodeCallback(AVFrame& frame)
{
    printf("onDecodeVideo\n");

    AVFrame* pDst = nullptr;
    pVideoScale->ScaleFrame(&frame, &pDst);

    rgbWriter.write((char*)pDst->data[0], pDst->linesize[0] * pDst->height);

    return 0;
}

int AudioDecodeCallback(AVFrame& frame)
{
    printf("onDecodeAudio\n");

    AVFrame* pDst = av_frame_alloc();

    pDst->channel_layout = AV_CH_LAYOUT_STEREO;
    pDst->format = AV_SAMPLE_FMT_S16;
    pDst->sample_rate = 48000;

    pAudioResampler->ResampleFrame(&frame, pDst);

    int dstBufSize = av_samples_get_buffer_size(pDst->linesize, pDst->ch_layout.nb_channels, pDst->nb_samples, (AVSampleFormat)pDst->format, 1);

    av_frame_free(&pDst);

    return 0;
}

int TestDecoder()
{
    int nRet = 0;

    rgbWriter.open("/home/cent/media/test.rgb", std::ios::binary);
    pcmWriter.open("/home/cent/media/test.pcm", std::ios::binary);

    FFDemuxer demuxer;
    demuxer.OpenUrl("/home/cent/media/src1.mp4");

    FFDecoder videoDecoder;
    videoDecoder.SetDecodeCallBack(VideoDecodeCallback);
    nRet = videoDecoder.Open(demuxer.GetVideoCodecParam(0));
    if (nRet < 0)
    {
        printf("open video codec error\n");
        return -1;
    }

    FFDecoder audioDecoder;
    audioDecoder.SetDecodeCallBack(AudioDecodeCallback);
    nRet = audioDecoder.Open(demuxer.GetAudioCodecParam(0));
    if (nRet < 0)
    {
        printf("open audio codec error\n");
        return -1;
    }

    pVideoScale = new FFScale(demuxer.GetVideoCodecParam(0)->width, demuxer.GetVideoCodecParam(0)->height,
        demuxer.GetVideoCodecParam(0)->format, 1920, 1080, AVPixelFormat::AV_PIX_FMT_RGB24);

    AVChannelLayout outLayout = AV_CHANNEL_LAYOUT_STEREO;
    pAudioResampler = new FFResample(&outLayout, AV_SAMPLE_FMT_S16, 48000,
        &demuxer.GetAudioCodecParam(0)->ch_layout, (AVSampleFormat)demuxer.GetAudioCodecParam(0)->format, demuxer.GetAudioCodecParam(0)->sample_rate);

    AVPacket pkt;
    FFUtils::InitPacket(&pkt);

    for (int i = 0; i < 100; i++)
    {
        nRet = demuxer.ReadPacket(pkt);
        if (nRet < 0)
        {
            printf("read packet error\n");
            return -1;
        }
        if (demuxer.IsVideoPacket(pkt, 0))
        {
            nRet = videoDecoder.SendPacket(pkt);
        }
        else if (demuxer.IsAudioPacket(pkt, 0))
        {
            nRet = audioDecoder.SendPacket(pkt);
        }

        av_packet_unref(&pkt);
    }

    videoDecoder.Close();
    audioDecoder.Close();

    rgbWriter.close();
    pcmWriter.close();

    if (pVideoScale)
    {
        delete pVideoScale;
        pVideoScale = nullptr;
    }

    if (pAudioResampler)
    {
        delete pAudioResampler;
        pAudioResampler = nullptr;
    }

    return 0;
}