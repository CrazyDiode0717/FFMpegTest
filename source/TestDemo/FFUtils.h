#pragma once

#include <string>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
}

class NonCopyable
{
protected:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;
};

namespace FFUtils
{
    std::string error2string(int error);

	void InitPacket(AVPacket* pkt);

    int FileLineSize(int linesizes[4], enum AVPixelFormat pixFmt, int width);
}