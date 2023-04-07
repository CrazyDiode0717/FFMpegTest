extern "C"
{
#include "libavutil/pixdesc.h"
}

void TestPixdesc()
{
    const AVPixFmtDescriptor* pDesc = av_pix_fmt_desc_get(AV_PIX_FMT_YUV420P);

    int bitsPerPixel = av_get_bits_per_pixel(pDesc);
    int padBits = av_get_padded_bits_per_pixel(pDesc);

    AVPixelFormat format = av_pix_fmt_desc_get_id(pDesc);

    int hShift = 0, vShift = 0;
    av_pix_fmt_get_chroma_sub_sample(format, &hShift, &vShift);

    int nPlanes = av_pix_fmt_count_planes(format);
}