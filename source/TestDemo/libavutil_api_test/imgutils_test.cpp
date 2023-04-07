#include <cstdio>
#include <cstdint>
#include <memory>

extern "C"
{
#include "libavutil/imgutils.h"
}


void TestImgutils()
{
    int nRet = 0;
    int nWidth = 1920;
    int nHeight = 1080;
    AVPixelFormat format = AV_PIX_FMT_YUV420P;
    int lineSize[4]{0};
    uint8_t* pData[4]{nullptr};
    size_t nPlaneSize[4]{0};

    for (int i = 0; i < 4; i++) 
    {
        nRet = av_image_get_linesize(format, nWidth, i);
        printf("linesize[%d]=%d\n", i, nRet);
    }

    nRet = av_image_fill_linesizes(lineSize, format, nWidth);
    for (int i = 0; i < 4; i++) 
    {
        printf("linesize[%d]=%d\n", i, lineSize[i]);
    }

    int imageSize = av_image_get_buffer_size(format, nWidth, nHeight, 1);
    printf("imageSize:%d\n", imageSize);

    std::unique_ptr<uint8_t[]> ptrImgBuf(new uint8_t[imageSize]);
    printf("ImageBuf:%p\n", ptrImgBuf.get());

    nRet = av_image_fill_pointers(pData, format, nHeight, ptrImgBuf.get(), lineSize);
    for(int i = 0; i < 4; i++)
    {
        printf("ImageBuf[%d]=%p\n", i, pData[i]);
    }

    nRet = av_image_fill_arrays(pData, lineSize, ptrImgBuf.get(), format, nWidth, nHeight, 1);
    for(int i = 0; i < 4; i++)
    {
        printf("ImageBuf[%d]=%p, linesize[%d]=%d\n", i, pData[i], i, lineSize[i]);
    }

    ptrdiff_t  llLineSize[4]{0};
    for(int i = 0; i < 4; i++)
        llLineSize[i] = lineSize[i];

    nRet = av_image_fill_plane_sizes(nPlaneSize, format, nHeight, llLineSize);
    for(int i = 0; i < 4; i++)
    {
        printf("planeSize[%d]=%d\n", i, nPlaneSize[i]);
    }

    nRet = av_image_fill_black(pData, llLineSize, format, AVCOL_RANGE_UNSPECIFIED, nWidth, nHeight);

    std::unique_ptr<uint8_t[]> ptrImgBuf1(new uint8_t[imageSize]);

    nRet = av_image_copy_to_buffer(ptrImgBuf1.get(), imageSize, pData, lineSize, format, nWidth, nHeight, 1);


    printf("TestImgutils\n");
}