extern "C"
{
#include "libavutil/macros.h"
}

#include <cstdint>
#include <cstdio>

void print_8()
{
    printf("8\n");
}

void print_10()
{
    printf("10\n");
}

#define BIT_COUNT 10

void TestMacros()
{
    int a[10]{0};
    int elements = FF_ARRAY_ELEMS(a);
    printf("elenents:%d\n", elements);

    uint32_t tag = MKTAG('a', 'v', 'C', 'c');
    uint32_t tag1 = MKBETAG('a', 'v', 'C', 'c'); 

    const char* p1 = AV_TOSTRING(12345);
    printf("%s\n", p1);

    AV_JOIN(print_, BIT_COUNT)();

    int aligned = FFALIGN(127, 64);
    printf("aligned:%d\n", aligned);
}