#pragma once


#include "FFUtils.h"

#include <string>
#include <vector>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

class FFDemuxer final : public NonCopyable
{
public:
    FFDemuxer();
    ~FFDemuxer();

public:
    int OpenUrl(const std::string& url);

    int AudioStreamCount() const;

    int VideoStreamCount() const;

    AVCodecParameters* GetAudioCodecParam(int streamIndex) const;

    AVCodecParameters* GetVideoCodecParam(int streamIndex) const;

    bool Seekable() const;

    int Seek(int64_t timestamp);

    int ReadPacket(AVPacket& pkt);

    bool IsAudioPacket(AVPacket& pkt, int streamIndex);

    bool IsVideoPacket(AVPacket& pkt, int streamIndex);

    void Close();

private:
    AVFormatContext *m_pFmtCtx;
    AVPacket m_pkt;

    std::vector<int> m_audioStreamIndexs;
    std::vector<int> m_videoStreamIndexs;
};
