#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "libavutil/fifo.h"
}

int TestFifo()
{
    printf("alloc fifo\n");
    AVFifo* hFifo = av_fifo_alloc2(10, sizeof(int), AV_FIFO_FLAG_AUTO_GROW);
    if(!hFifo)
    {
        printf("hFifo is nullptr\n");
        return 0;
    }

    av_fifo_auto_grow_limit(hFifo, 20);

    size_t curSize = av_fifo_elem_size(hFifo);
    printf("fifo size:%d\n", curSize);
    size_t sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    size_t sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("write 5 elements\n");
    for(int i = 0; i < 5; i++)
        av_fifo_write(hFifo, &i, 1);
    
    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("write 10 elements\n");
    for(int i = 0; i < 10; i++)
        av_fifo_write(hFifo, &i, 1);

    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("read 10 elements\n");
    int buf[10]{0};
    av_fifo_read(hFifo, buf, 10);

    for(int i = 0 ; i < 10; i++)
        printf("%d ", buf[i]);
    printf("\n");

    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("peek 3 elements\n");
    av_fifo_peek(hFifo, buf, 3, 2);
    for(int i = 0; i < 3; i++)
        printf("%d ", buf[i]);
    printf("\n");

    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("drain 3 elements\n");
    av_fifo_drain2(hFifo, 3);
    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    printf("reset\n");
    av_fifo_reset2(hFifo);
    sizeCanRead = av_fifo_can_read(hFifo);
    printf("size can read:%d\n", sizeCanRead);
    sizeCanWrite = av_fifo_can_write(hFifo);
    printf("size can write:%d\n", sizeCanWrite);

    av_fifo_freep2(&hFifo);

    return 0;
}