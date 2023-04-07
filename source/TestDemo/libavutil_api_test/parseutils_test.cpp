
extern "C"
{
#include "libavutil/parseutils.h"
}

#include <cstdio>
#include <inttypes.h>

void TestParseutils()
{
    int nRet = 0;

    int nWidth = 0, nHeight = 0;
    nRet = av_parse_video_size(&nWidth, &nHeight, "uhd2160");

    AVRational r{0, 0};
    nRet = av_parse_video_rate(&r, "ntsc");

    int64_t timeVal = 0;
    nRet = av_parse_time(&timeVal, "2023-03-27T19:14:24Z", 0);

    time_t tvi = timeVal / 1000000;
    tm* pTm = gmtime(&tvi);
    printf("%14"PRIi64".%06d = %04d-%02d-%02dT%02d:%02d:%02dZ\n",
           timeVal / 1000000, (int)(timeVal % 1000000), pTm->tm_year + 1900,
           pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min,
           pTm->tm_sec);
}