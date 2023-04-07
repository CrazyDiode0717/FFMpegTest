#include "FFUtils.h"

std::string FFUtils::error2string(int error)
{
    char errorBuf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    av_strerror(error, errorBuf, AV_ERROR_MAX_STRING_SIZE);
    return std::string(errorBuf);
}

void FFUtils::InitPacket(AVPacket* pkt)
{
    if (pkt)
    {
        av_init_packet(pkt);
        pkt->data = nullptr;
        pkt->size = 0;
    }
}

int FFUtils::FileLineSize(int linesizes[4], AVPixelFormat pixFmt, int width)
{
    return av_image_fill_linesizes(linesizes, pixFmt, width);
}
