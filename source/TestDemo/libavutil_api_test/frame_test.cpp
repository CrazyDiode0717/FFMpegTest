extern "C"
{
#include "libavutil/frame.h"
}

void TestFrame()
{
    int nRet = 0;

    AVFrame* pFrame = av_frame_alloc();

    pFrame->format = AV_PIX_FMT_YUV420P;
    pFrame->width = 1920;
    pFrame->height = 1080;
    av_frame_get_buffer(pFrame, 16);

    AVFrame* pFrame1 = av_frame_alloc();
    av_frame_ref(pFrame1, pFrame);
    av_frame_unref(pFrame1);
    av_frame_free(&pFrame1);

    AVFrame* pFrame2 = av_frame_clone(pFrame);
    nRet = av_frame_is_writable(pFrame2);
    nRet = av_frame_make_writable(pFrame2);
    av_frame_free(&pFrame2);

    AVFrame* pFrame3 = av_frame_alloc();
    pFrame3->format = AV_PIX_FMT_YUV420P;
    pFrame3->width = 1920;
    pFrame3->height = 1080;
    av_frame_get_buffer(pFrame3, 16);
    av_frame_copy_props(pFrame3, pFrame);
    av_frame_copy(pFrame3, pFrame);
    av_frame_free(&pFrame3);

    AVBufferRef* pBuf =  av_frame_get_plane_buffer(pFrame, 0);
    //av_buffer_unref(&pBuf);

    AVFrame* pFrame4 = av_frame_alloc();
    av_frame_move_ref(pFrame4, pFrame);
    av_frame_free(&pFrame4);

    av_frame_free(&pFrame);
}