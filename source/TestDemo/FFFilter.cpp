#include "FFFilter.h"
#include <sstream>

extern "C"
{
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
}

//���Բο�ffmpeg_filter.c configure_filtergraph(FilterGraph *fg)

FFFilter::FFFilter() :m_pfilterGraph(nullptr)
{
}

FFFilter::~FFFilter()
{
	Deinit();
}

int FFFilter::CreateFilterGraph(const char* complex)
{
	int nRet = 0;
	AVFilterInOut* inputs = nullptr;
	AVFilterInOut* outputs = nullptr;

	Deinit();

	m_pfilterGraph = avfilter_graph_alloc();
	if (!m_pfilterGraph)
	{
		printf("failed to allocate filter graph");
		return 0;
	}

	nRet = avfilter_graph_parse2(m_pfilterGraph, complex, &inputs, &outputs);
	if (nRet < 0)
	{
		printf("failed to parse filter ptr");
		return 0;
	}

	for (auto cur = inputs; cur; cur = cur->next)
	{
		const AVFilter* buffersrc = avfilter_get_by_name("buffer");

		char args[512]{ 0 };
		sprintf(args,
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
			1920, 1080, AV_PIX_FMT_YUV420P,
			1, 25,
			1920, 1080);

		char name[255]{0};
		sprintf(name, "buffer_%lld", m_vecInputFilter.size());

		AVFilterContext* pInputFilterContext = nullptr;
		int ret = avfilter_graph_create_filter(&pInputFilterContext, buffersrc, name, args,
			nullptr, m_pfilterGraph);
		if (ret < 0)
		{
			printf("failed to create main filter");
			return ret;
		}
		ret = avfilter_link(pInputFilterContext, 0, cur->filter_ctx, cur->pad_idx);

		m_vecInputFilter.push_back(pInputFilterContext);
	}

	avfilter_inout_free(&inputs);

	for (auto cur = outputs; cur; cur = cur->next)
	{
		const AVFilter* buffersink = avfilter_get_by_name("buffersink");

		AVFilterContext* pOutputFilterContext = nullptr;
		nRet = avfilter_graph_create_filter(&pOutputFilterContext, buffersink, "output", nullptr,
			nullptr, m_pfilterGraph);
		if (nRet < 0)
		{
			printf("failed to create output filter");
			return nRet;
		}
		nRet = avfilter_link(cur->filter_ctx, cur->pad_idx, pOutputFilterContext, 0);

		m_vecOutputFilter.push_back(pOutputFilterContext);
	}

	avfilter_inout_free(&outputs);

	nRet = avfilter_graph_config(m_pfilterGraph, nullptr);
	if (nRet < 0)
	{
		printf("failed to config filter");
		return nRet;
	}

	char* strGraph = avfilter_graph_dump(m_pfilterGraph, nullptr);

	printf("%s\n", strGraph);

	return 1;
}

int FFFilter::CreateFilterGraph2(const char* complex)
{
	int nRet = 0;
	AVFilterInOut* inputs = nullptr;
	AVFilterInOut* outputs = nullptr;

	Deinit();

	m_pfilterGraph = avfilter_graph_alloc();
	if (!m_pfilterGraph)
	{
		printf("failed to allocate filter graph");
		return 0;
	}

	nRet = avfilter_graph_parse2(m_pfilterGraph, complex, &inputs, &outputs);
	if (nRet < 0)
	{
		printf("failed to parse filter ptr");
		return 0;
	}

	nRet = avfilter_graph_config(m_pfilterGraph, nullptr);
	if (nRet < 0)
	{
		printf("failed to config filter");
		return nRet;
	}

	char* strGraph = avfilter_graph_dump(m_pfilterGraph, nullptr);

	AVFilterContext* pInput1 = avfilter_graph_get_filter(m_pfilterGraph, "Parsed_buffer_0");
	AVFilterContext* pInput2 = avfilter_graph_get_filter(m_pfilterGraph, "Parsed_buffer_1");

	AVFilterContext* pOutput = avfilter_graph_get_filter(m_pfilterGraph, "Parsed_buffersink_3");

	return 1;
}

void FFFilter::SetOutputCallBack(std::function<int(AVFrame*)> callback)
{
	m_callback = callback;
}

int FFFilter::SendFrame(AVFrame* pFrame)
{
	int nRet = 0;

	nRet = av_buffersrc_add_frame_flags(m_vecInputFilter[0], pFrame, AV_BUFFERSRC_FLAG_PUSH);
	if (nRet >= 0)
	{
		nRet = av_buffersrc_add_frame_flags(m_vecInputFilter[1], pFrame, AV_BUFFERSRC_FLAG_PUSH);
		if (nRet >= 0)
		{
			while (true)
			{
				AVFrame filterOutFrame;
				nRet = av_buffersink_get_frame_flags(m_vecOutputFilter[0], &filterOutFrame, AV_BUFFERSINK_FLAG_NO_REQUEST);
				if (nRet == AVERROR(EAGAIN) || nRet == AVERROR_EOF)
					break;
				else if (nRet < 0)
					return nRet;

				m_callback(&filterOutFrame);
			}
		}
	}
	return 0;
}

void FFFilter::Deinit()
{
	if (m_pfilterGraph)
	{
		avfilter_graph_free(&m_pfilterGraph);
		m_pfilterGraph = nullptr;
	}
}