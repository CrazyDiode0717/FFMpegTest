#pragma once

extern "C"
{
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <cstdint>

class FFResample
{
public:
	FFResample(AVChannelLayout* outChannelLayout, AVSampleFormat outFormat, int outSampleRate, AVChannelLayout* inChannelLayout, AVSampleFormat inFormat, int inSampleRate);
	~FFResample();

	int ResampleFrame(AVFrame* src, AVFrame* dst);

private:
	SwrContext* m_pContext;
};