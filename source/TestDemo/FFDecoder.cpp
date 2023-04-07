#include "FFDecoder.h"
#include "FFUtils.h"
#include <iostream>

FFDecoder::FFDecoder() : m_pCodecContext(nullptr), m_pFrame(nullptr)
{
}

FFDecoder::~FFDecoder()
{
    if(m_pCodecContext)
    {
        avcodec_free_context(&m_pCodecContext);
        m_pCodecContext = nullptr;
    }

    if (m_pFrame)
    {
        av_frame_free(&m_pFrame);
        m_pFrame = nullptr;
    }
}

int FFDecoder::Open(AVCodecParameters *param)
{
    if(!param)
        return -1;
    const AVCodec *pCodec = avcodec_find_decoder(param->codec_id);
    if (!pCodec)
    {
        avcodec_parameters_free(&param);
        std::cout << "can't find the codec id " << param->codec_id << std::endl;
        return -2;
    }


    m_pCodecContext = avcodec_alloc_context3(pCodec);

    avcodec_parameters_to_context(m_pCodecContext, param);

    m_pCodecContext->thread_count = 8;

    int ret = avcodec_open2(m_pCodecContext, nullptr, nullptr);
    if (ret != 0)
    {
        std::cout << "avcodec_open2  failed!" << std::endl;
        return -3;
    }

    if(!m_pFrame)
        m_pFrame = av_frame_alloc();

    return 0;
}

void FFDecoder::SetDecodeCallBack(std::function<int(AVFrame&)> callback)
{
    m_decodeCallback = callback;
}

int FFDecoder::SendPacket(AVPacket& packet)
{
    int nRet = 0;
    nRet = avcodec_send_packet(m_pCodecContext, &packet);
    if (nRet < 0)
    {
        printf("Error sending a packet for decoding\n");
        return nRet;
    }

    while (nRet >= 0)
    {
        nRet = avcodec_receive_frame(m_pCodecContext, m_pFrame);

        if (nRet == AVERROR(EAGAIN) || nRet == AVERROR_EOF)
            return 0;
        else if (nRet < 0) 
        {
            printf("Error during decoding\n");
            return nRet;
        }

        m_decodeCallback(*m_pFrame);

        av_frame_unref(m_pFrame);
    }

    return 1;
}

int FFDecoder::Close()
{
    int nRet = 0;
    AVPacket packet;

    FFUtils::InitPacket(&packet);

    nRet = avcodec_send_packet(m_pCodecContext, &packet);
    if (nRet < 0)
    {
        printf("Error sending a packet for decoding\n");
        return nRet;
    }

    while (nRet >= 0)
    {
        nRet = avcodec_receive_frame(m_pCodecContext, m_pFrame);
        if (nRet == AVERROR(EAGAIN) || nRet == AVERROR_EOF)
            return 0;
        else if (nRet < 0) {
            printf("Error during decoding\n");
            return nRet;
        }

        m_decodeCallback(*m_pFrame);

        av_frame_unref(m_pFrame);

        return nRet;
    }

    return 0;
}
