
#include "FFFilter.h"
#include <fstream>
#include <memory>

int TestFilter()
{
    int nRet = 0;

    FFFilter filter;
    //const char* filter_complex = "overlay=x=0:y=0:format=yuv422";
    const char* filter_complex = "[0]scale=720:576[back];[1][back]overlay=x=0:y=0:format=yuv422";
    //const char* filter_complex = "[0][1]overlay=x=0:y=0:format=yuv422";
    nRet = filter.CreateFilterGraph(filter_complex);

	filter.SetOutputCallBack([](AVFrame* pFrame) {
        std::ofstream yuvWriter("/home/cent/media/filter.yuv", std::ios::binary);
        yuvWriter.write((char*)pFrame->data[0], pFrame->linesize[0] * pFrame->height);
        yuvWriter.write((char*)pFrame->data[1], pFrame->linesize[1] * pFrame->height / 2);
        yuvWriter.write((char*)pFrame->data[2], pFrame->linesize[2] * pFrame->height / 2);
        yuvWriter.close();
		return 0; });

    int width = 1920;
    int height = 1080;
    int nFrameLen = width * height * 3 / 2;
    std::unique_ptr<uint8_t[]> ptrBuf(new uint8_t[nFrameLen]);
    std::ifstream yuvReader("/home/cent/media/1920x1080.yuv", std::ios::binary);
    yuvReader.read((char*)ptrBuf.get(), nFrameLen);
    yuvReader.close();

    AVFrame *pFrame = av_frame_alloc();
    pFrame->width = width;
    pFrame->height = height;
    pFrame->format = AV_PIX_FMT_YUV420P;
    pFrame->pts = 1;
    pFrame->linesize[0] = pFrame->width;
    pFrame->linesize[1] = pFrame->width / 2;
    pFrame->linesize[2] = pFrame->width / 2;
    pFrame->data[0] = ptrBuf.get();
    pFrame->data[1] = pFrame->data[0] + pFrame->linesize[0] * pFrame->height;
    pFrame->data[2] = pFrame->data[1] + pFrame->linesize[1] * pFrame->height / 2;

    filter.SendFrame(pFrame);

    // 
    //const char* filter_complex = "buffer=video_size=720x576:pix_fmt=2:time_base=1/25:pixel_aspect=16/9 [in_1];[in_1]scale=1920:1080[back]; buffer=video_size=720x576:pix_fmt=0:time_base=1/25:pixel_aspect=16/9 [in_2]; [back] [in_2] overlay=0:0 [result]; [result] buffersink";
    //const char* filter_complex = "buffer=video_size=1024x768:pix_fmt=2:time_base=1/25:pixel_aspect=3937/3937[in_1];buffer=video_size=1920x1080:pix_fmt=0:time_base=1/180000:pixel_aspect=0/1[in_2];[in_1][in_2]overlay=0:0[result];[result]buffersink";
    //nRet = filter.CreateFilterGraph(filter_complex);

    return 0;
}