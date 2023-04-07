#pragma once

#include <functional>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class FFDecoder
{
public:
    FFDecoder();
    ~FFDecoder();
    FFDecoder(const FFDecoder&) = delete;
    FFDecoder& operator=(const FFDecoder&) = delete;

    void SetDecodeCallBack(std::function<int(AVFrame&)> callback);

    int Open(AVCodecParameters *param);

    int SendPacket(AVPacket& packet);

    int Close();

private:
    AVCodecContext *m_pCodecContext;
    std::function<int(AVFrame&)> m_decodeCallback;
    AVFrame* m_pFrame;
};
