#pragma once


#include "FFUtils.h"

extern "C"
{
#include "libavfilter/avfilter.h"
}

#include <string>
#include <vector>
#include <functional>

class FFFilter : public NonCopyable
{
public:
	FFFilter();
	~FFFilter();

public:
	int CreateFilterGraph(const char* complex);

	int CreateFilterGraph2(const char* complex);

	void SetOutputCallBack(std::function<int(AVFrame*)> callback);

	int SendFrame(AVFrame* pFrame);
private:
	void Deinit();
	
private:
	AVFilterGraph* m_pfilterGraph;;

	std::vector<AVFilterContext*> m_vecInputFilter;
	std::vector<AVFilterContext*> m_vecOutputFilter;

	std::function<int(AVFrame*)> m_callback;
};