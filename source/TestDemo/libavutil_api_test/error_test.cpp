extern "C"
{
#include "libavutil/error.h"
}

#ifdef av_err2str
#undef av_err2str
#include <string>
std::string av_err2string(int errnum)
{
    char str[AV_ERROR_MAX_STRING_SIZE];
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#define av_err2str(err) av_err2string(err).c_str()
#endif  // av_err2str

void TestError()
{
    printf("error:%s\n", av_err2str(AVERROR_PROTOCOL_NOT_FOUND));
}