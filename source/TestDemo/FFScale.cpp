#include "FFScale.h"

FFScale::FFScale(int srcWidth, int srcHeight, int srcPixFmt, int dstWidth, int dstHeight, int dstPixFmt)
    : m_pSwsCtx(nullptr)
{
    m_pSwsCtx = sws_getContext(srcWidth, srcHeight, (AVPixelFormat)srcPixFmt,
        dstWidth, dstHeight, (AVPixelFormat)dstPixFmt,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!m_pSwsCtx)
    {
        printf("create sws ctx failed\n");
        return;
    }

    m_pFrame = av_frame_alloc();
    m_pFrame->format = dstPixFmt;
    m_pFrame->width = dstWidth;
    m_pFrame->height = dstHeight;

    int nRet = av_frame_get_buffer(m_pFrame, 64);
}

FFScale::~FFScale()
{
    if (m_pSwsCtx)
    {
        sws_freeContext(m_pSwsCtx);
        m_pSwsCtx = nullptr;
    }
}

int FFScale::ScaleFrame(AVFrame* src, AVFrame** dst)
{
    int nRet = sws_scale(m_pSwsCtx, src->data, src->linesize, 0, src->height, m_pFrame->data, m_pFrame->linesize);

    if (nRet != m_pFrame->height)
    {
        return -1;
    }

    *dst = m_pFrame;

    return 1;
}


