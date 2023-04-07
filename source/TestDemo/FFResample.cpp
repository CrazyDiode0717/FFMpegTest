#include "FFResample.h"

FFResample::FFResample(AVChannelLayout* outChannelLayout, AVSampleFormat outFormat, int outSampleRate, AVChannelLayout* inChannelLayout, AVSampleFormat inFormat, int inSampleRate)
    : m_pContext(nullptr)
{
    swr_alloc_set_opts2(&m_pContext, outChannelLayout, outFormat, outSampleRate, inChannelLayout, inFormat, inSampleRate, 0, nullptr);

    if (0 != swr_init(m_pContext))
    {
        swr_free(&m_pContext);
        m_pContext =  nullptr;
    }
}

FFResample::~FFResample()
{
    if (m_pContext)
    {
        swr_free(&m_pContext);
        m_pContext = nullptr;
    }
}

int FFResample::ResampleFrame(AVFrame* src, AVFrame* dst)
{
    int nRet = 0;
    if (!m_pContext || !src || !dst)
        return nRet;
    
    nRet = swr_convert_frame(m_pContext, dst, src);

    return nRet;
}
