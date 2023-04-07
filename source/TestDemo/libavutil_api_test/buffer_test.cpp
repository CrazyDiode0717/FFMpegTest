extern "C"
{
    #include "libavutil/buffer.h"
}

#include <cstdio>

struct stCtx
{
    int n;
};

AVBufferRef* pool_alloc(void *opaque, size_t size);


void TestBuffer()
{
    stCtx ctx;
    ctx.n = 1;

    AVBufferPool* pBufferPoll = av_buffer_pool_init2(10, &ctx, pool_alloc, NULL);
    if (!pBufferPoll)
        return;
    
    AVBufferRef* pBuf = av_buffer_pool_get(pBufferPoll);
    if(!pBuf)
        return;

    printf("pBuf:%p\n", pBuf);
    
    AVBufferRef* pBuf1 = av_buffer_ref(pBuf);

    int nRefCount = av_buffer_get_ref_count(pBuf);
    printf("refCount:%d\n", nRefCount);

    av_buffer_unref(&pBuf1);

    nRefCount = av_buffer_get_ref_count(pBuf);
    printf("refCount:%d\n", nRefCount);

    int nWriteable = av_buffer_is_writable(pBuf);

    av_buffer_make_writable(&pBuf);

    nWriteable = av_buffer_is_writable(pBuf);

    av_buffer_unref(&pBuf);

    pBuf = av_buffer_pool_get(pBufferPoll);

    printf("pBuf:%p\n", pBuf);

    stCtx* pCtx = (stCtx*)av_buffer_pool_buffer_get_opaque(pBuf);

    av_buffer_pool_uninit(&pBufferPoll);
}

void buffer_free(void *opaque, uint8_t *data)
{
    stCtx* ctx = (stCtx*)opaque;
    
    if(data)
        delete[] data;
}

AVBufferRef* pool_alloc(void *opaque, size_t size)
{
    stCtx* ctx = (stCtx*)opaque;

    AVBufferRef *ret = nullptr;
    uint8_t* pData = nullptr;
    
    pData = new uint8_t[size];

    ret = av_buffer_create(pData, size, buffer_free, ctx, 0);
    if (!ret) 
    {
        return ret;
    }

    return ret;
}