#include <ctime>
#include <cstdio>
#include <cstring>

extern "C"
{
#include "libavutil/bprint.h"
}

static void srt_print(AVBPrint* b, const char *str, ...)
{
    va_list vargs;
    va_start(vargs, str);
    av_vbprintf(b, str, vargs);
    va_end(vargs);
}

void TestBprint()
{
    AVBPrint b;

    av_bprint_init(&b, 0, AV_BPRINT_SIZE_UNLIMITED);

    srt_print(&b, "%s,%d", "test", 10);
    av_bprint_chars(&b, 'a', 3);
    av_bprint_append_data(&b, "hello", strlen("hello") + 1);

    printf("%s\n", b.str);
    av_bprint_finalize(&b, NULL);

    av_bprint_init(&b, 0, AV_BPRINT_SIZE_UNLIMITED);

    time_t now = time(0);
    tm testtime;
    localtime_r(&now, &testtime);
    av_bprint_strftime(&b, "%Y-%m-%d %H:%M:%S", &testtime);
    
    printf("%s\n", b.str);
    av_bprint_finalize(&b, NULL);
}