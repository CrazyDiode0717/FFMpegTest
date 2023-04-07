#include <cstdio>
extern "C"
{
    #include "libavutil/avstring.h"
    #include "libavutil/mem.h"
}

void TestAvstring()
{
    const char* str = "Test FFmpeg";
    int nRet = 0;

    const char* ptr = nullptr;
    nRet = av_strstart(str, "Test", &ptr);
    nRet = av_stristart(str, "test", &ptr);

    ptr = av_stristr(str, "Es");

    ptr = av_strnstr(str, "st", 4);

    char buf[20]{0};
    size_t sizBuf = av_strlcpy(buf, str, 5); 

    sizBuf = av_strlcat(buf, str, 20);

    sizBuf = av_strlcatf(buf, 20, "%d", 10);

    sizBuf = av_strnlen(buf, 20);

    char* p = av_asprintf("%d-%d", 10, 11);

    av_free(p);
    p = nullptr;

    const char* str1 = "http://test.com";

    //从*buf字符串找到第一个出现在term中的任意一个字符，将*buf进行截断，前面一部分(不包含term中的任意一个字符)通过返回值反馈，后面一部分在*buf反馈。
    p = av_get_token(&str1, ":");

    char* p1 = nullptr;
    p = av_strtok("abc,def,ghi",",", &p1);

    printf("TestAvstring\n");
}