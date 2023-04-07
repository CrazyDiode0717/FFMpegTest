extern "C"
{
#include "libavformat/avio.h"
#include "libavutil/mem.h"
}
#include <cstdio>

#ifdef av_err2str
#undef av_err2str
#include <string>
extern std::string av_err2string(int errnum);
#define av_err2str(err) av_err2string(err).c_str()
#endif  // av_err2str

void show_protocols()
{
    void* opaque = NULL;
    const char* name;

    printf("Supported file protocols:\n");

    printf("Input:\n");
    while ((name = avio_enum_protocols(&opaque, 0)))
        printf("  %s\n", name);

    printf("Output:\n");
    while ((name = avio_enum_protocols(&opaque, 1)))
        printf("  %s\n", name);
}

const char* type_string(int type)
{
    switch (type) {
    case AVIO_ENTRY_DIRECTORY:
        return "<DIR>";
    case AVIO_ENTRY_FILE:
        return "<FILE>";
    case AVIO_ENTRY_BLOCK_DEVICE:
        return "<BLOCK DEVICE>";
    case AVIO_ENTRY_CHARACTER_DEVICE:
        return "<CHARACTER DEVICE>";
    case AVIO_ENTRY_NAMED_PIPE:
        return "<PIPE>";
    case AVIO_ENTRY_SYMBOLIC_LINK:
        return "<LINK>";
    case AVIO_ENTRY_SOCKET:
        return "<SOCKET>";
    case AVIO_ENTRY_SERVER:
        return "<SERVER>";
    case AVIO_ENTRY_SHARE:
        return "<SHARE>";
    case AVIO_ENTRY_WORKGROUP:
        return "<WORKGROUP>";
    case AVIO_ENTRY_UNKNOWN:
    default:
        break;
    }
    return "<UNKNOWN>";
}

int ReadCB(void *opaque, uint8_t *buf, int buf_size)
{
    printf("ReadCB: %d, handle: %d\n", buf_size, *(uint8_t*)opaque);
    return buf_size;
}

int WriteCB(void *opaque, uint8_t *buf, int buf_size)
{
    printf("WriteCB: %d, handle: %d\n", buf_size, *(uint8_t*)opaque);
    return buf_size;
}

int64_t SeekCB(void *opaque, int64_t offset, int whence)
{
    printf("SeekCB: %ld, handle: %d\n", offset, *(uint8_t*)opaque);
    switch (whence)
	{
	case AVSEEK_SIZE:
		return 100;
	case SEEK_SET:
        return offset;
	case SEEK_CUR:
		return offset;
	default:
		return AVERROR(EINVAL);
	}
	return 0;
}

void TestAvio()
{
    char* srcName = "/home/cent/media/src1.mp4";
    char* dstName = "/home/cent/media/test_avio.mp4";
    AVIOContext* pReadCtx = NULL;
    AVIOContext* pWriteCtx = NULL;
    unsigned char buf[1024];
    int nRet = 0;

    show_protocols();

    const char* protocolName = avio_find_protocol_name(srcName);
    printf("protocol name: %s\n", protocolName);

    nRet = avio_open2(&pReadCtx, srcName, AVIO_FLAG_READ, nullptr, nullptr);
    nRet = avio_open2(&pWriteCtx, dstName, AVIO_FLAG_WRITE, nullptr, nullptr);

    int64_t srcSize = avio_size(pReadCtx);
    printf("srcSize: %lld\n", srcSize);

    nRet = avio_read(pReadCtx, buf, 1024);

    int64_t pos = avio_tell(pReadCtx);
    nRet = avio_seek(pReadCtx, 0, SEEK_SET);
    pos = avio_tell(pReadCtx);
    nRet = avio_skip(pReadCtx, 2048);
    pos = avio_tell(pReadCtx);

    avio_write(pWriteCtx, buf, 1024);
    avio_wb32(pWriteCtx, 'avCc');
    //avio_flush(pWriteCtx);

    avio_printf(pWriteCtx, "#EXTM3U\n");
    avio_printf(pWriteCtx, "#EXT-X-VERSION:%d\n", 3);

    nRet = avio_close(pReadCtx);
    nRet = avio_close(pWriteCtx);

    int bufSize = 100;
    uint8_t handle = 10;
    AVIOContext* pCtx1 = avio_alloc_context((unsigned char *)av_malloc(bufSize), bufSize, 1, &handle, ReadCB, WriteCB, SeekCB);
    avio_write(pCtx1, buf, 1024);
    avio_flush(pCtx1);
    av_free(pCtx1->buffer);
	avio_context_free(&pCtx1);

    AVIOContext* pWriteOnlyBufCtx = NULL;
    uint8_t* pWriteBuf = NULL;
    uint8_t* pWriteBuf1 = NULL;

    nRet = avio_open_dyn_buf(&pWriteOnlyBufCtx);
    avio_write(pWriteOnlyBufCtx, buf, 1024);
    //int nSize = avio_get_dyn_buf(pWriteOnlyBufCtx, &pWriteBuf);
    nRet = avio_close_dyn_buf(pWriteOnlyBufCtx, &pWriteBuf1);

    av_free(pWriteBuf1);

    nRet = avio_check(srcName, AVIO_FLAG_READ);

#if 0
    AVIODirEntry* entry = NULL;
    AVIODirContext* ctx = NULL;
    int cnt, ret;
    char filemode[4], uid_and_gid[20];

    if ((ret = avio_open_dir(&ctx, "/home/cent/media/", NULL)) < 0)
    {
        printf("Cannot open directory: %s.\n", av_err2str(ret));
        avio_close_dir(&ctx);
        return;
    }

    cnt = 0;
    for (;;)
    {
        if ((ret = avio_read_dir(ctx, &entry)) < 0)
        {
            printf("Cannot list directory: %s.\n", av_err2str(ret));
            avio_close_dir(&ctx);
            return;
        }
        if (!entry)
            break;
        if (entry->filemode == -1)
        {
            snprintf(filemode, 4, "???");
        }
        else
        {
            snprintf(filemode, 4, "%3"PRIo64, entry->filemode);
        }
        snprintf(uid_and_gid, 20, "%"PRId64"(%"PRId64")", entry->user_id, entry->group_id);
        if (cnt == 0)
            printf("%-9s %12s %30s %10s %s %16s %16s %16s\n",
                "TYPE", "SIZE", "NAME", "UID(GID)", "UGO", "MODIFIED",
                "ACCESSED", "STATUS_CHANGED");
        printf("%-9s %12"PRId64" %30s %10s %s %16"PRId64" %16"PRId64" %16"PRId64"\n",
            type_string(entry->type),
            entry->size,
            entry->name,
            uid_and_gid,
            filemode,
            entry->modification_timestamp,
            entry->access_timestamp,
            entry->status_change_timestamp);
        avio_free_directory_entry(&entry);
        cnt++;
    }
#endif

}