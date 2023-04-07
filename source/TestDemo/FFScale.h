#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
}

class FFScale
{
public:
	FFScale(int srcWidth, int srcHeight, int srcPixFmt, int dstWidth, int dstHeight, int dstPixFmt);
	~FFScale();

	int ScaleFrame(AVFrame* src, AVFrame** dst);

private:
	SwsContext* m_pSwsCtx;
	AVFrame* m_pFrame;
};