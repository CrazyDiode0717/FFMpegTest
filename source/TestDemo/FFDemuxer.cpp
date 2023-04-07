#include "FFDemuxer.h"
#include "FFUtils.h"

FFDemuxer::FFDemuxer() : m_pFmtCtx(nullptr)
{
    avformat_network_init();

    FFUtils::InitPacket(&m_pkt);
}

FFDemuxer::~FFDemuxer()
{
    if (m_pFmtCtx)
    {
        avformat_close_input(&m_pFmtCtx);
        m_pFmtCtx = nullptr;
    }

    avformat_network_deinit();
}

int FFDemuxer::OpenUrl(const std::string& url)
{
    int nRet = 0;

    AVDictionary* pOptions = nullptr;
    //av_dict_set(&pOptions, "protocol_blacklist", "file,http,https,rtp,udp,tcp,tls", 0);

    nRet = avformat_open_input(&m_pFmtCtx, url.c_str(), nullptr, &pOptions);
    if (pOptions != nullptr)
    {
        av_dict_free(&pOptions);
    }
    if (nRet < 0)
    {
        printf("Could not open source file %s\n", url.c_str());
        return nRet;
    }

    nRet = avformat_find_stream_info(m_pFmtCtx, nullptr);
    if (nRet < 0)
    {
        printf("Could not find stream information\n");
        return nRet;
    }

    for (int nStream = 0; nStream < m_pFmtCtx->nb_streams; nStream++)
    {
        switch (m_pFmtCtx->streams[nStream]->codecpar->codec_type)
        {
        case AVMediaType::AVMEDIA_TYPE_VIDEO:
            m_videoStreamIndexs.push_back(nStream);
            break;
        case AVMediaType::AVMEDIA_TYPE_AUDIO:
            m_audioStreamIndexs.push_back(nStream);
            break;
        default:
            break;
        }
    }

    return 0;
}

int FFDemuxer::AudioStreamCount() const
{
    return m_audioStreamIndexs.size();
}

int FFDemuxer::VideoStreamCount() const
{
    return m_videoStreamIndexs.size();
}

AVCodecParameters* FFDemuxer::GetAudioCodecParam(int streamIndex) const
{
    return m_pFmtCtx->streams[m_audioStreamIndexs[streamIndex]]->codecpar;
}

AVCodecParameters* FFDemuxer::GetVideoCodecParam(int streamIndex) const
{
    return m_pFmtCtx->streams[m_videoStreamIndexs[streamIndex]]->codecpar;
}

bool FFDemuxer::Seekable() const
{
    return m_pFmtCtx->pb->seekable;
}

int FFDemuxer::Seek(int64_t timestamp)
{
    if (Seekable())
    {
        int nRet = avformat_seek_file(m_pFmtCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);
        if (nRet < 0) 
        {
            printf("%s: could not seek to position %0.3f\n", m_pFmtCtx->url, (double)timestamp / AV_TIME_BASE);
            return nRet;
        }
    }
    return 0;
}

int FFDemuxer::ReadPacket(AVPacket& pkt)
{
    int nRet = av_read_frame(m_pFmtCtx, &m_pkt);

    //av_pkt_dump_log2(NULL, AV_LOG_INFO, &m_pkt, 1, m_pFmtCtx->streams[pkt.stream_index]);

    if(nRet >= 0)
    {
        av_packet_ref(&pkt, &m_pkt);
        av_packet_unref(&m_pkt);

        return 0;
    }

    return -1;
}

bool FFDemuxer::IsAudioPacket(AVPacket& pkt, int streamIndex)
{
    return pkt.stream_index == m_audioStreamIndexs[streamIndex];
}

bool FFDemuxer::IsVideoPacket(AVPacket& pkt, int streamIndex)
{
    return pkt.stream_index == m_videoStreamIndexs[streamIndex];
}

void FFDemuxer::Close()
{
    if (m_pFmtCtx)
    {
        avformat_close_input(&m_pFmtCtx);
        m_pFmtCtx = nullptr;
    }
}
