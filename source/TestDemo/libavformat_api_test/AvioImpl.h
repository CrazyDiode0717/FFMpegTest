#pragma once

extern "C"
{
#include "libavformat/avio.h"
#include "libavformat/avformat.h"
}

#include <stdio.h>

class CVXAvIO
{
public:
	CVXAvIO() : m_pAvio(nullptr), m_hFile(nullptr)
	{
        memset(&m_Opa, 0, sizeof(stOpaque));
	}
    ~CVXAvIO()
    {
        if (m_pAvio)
        {
            av_free(m_pAvio->buffer);
            avio_context_free(&m_pAvio);

            m_pAvio = nullptr;
        }

        if (m_hFile)
        {
            fclose(m_hFile);
            m_hFile = nullptr;
        }

        m_Opa.nWriteFlag = 0;
    }
    int SetFileName(const char* lpFileName, bool bWrite)
    {
        if (m_hFile)
        {
            fclose(m_hFile);
            m_hFile = nullptr;
        }

        m_Opa.nWriteFlag = 0;
        m_Opa.nPos = 0;

        if (lpFileName == nullptr)
        {
            return 0;
        }

        m_hFile = fopen(lpFileName, "wb");
        if (m_hFile == nullptr)
        {
            return -1;
        }

        m_nBufSize = 2 * 1024 * 1024;

        if (bWrite)
        {
            m_Opa.nWriteFlag = 1;
        }
        else
        {
            m_Opa.nWriteFlag = 0;
        }

        m_Opa.ptr = m_hFile;

        m_pAvio = avio_alloc_context((unsigned char*)av_malloc(m_nBufSize), m_nBufSize, m_Opa.nWriteFlag, &m_Opa, this->ReadCB, this->WriteCB, this->SeekCB);
        if (m_pAvio == nullptr)
        {
            return -2;
        }

        return 1;
    }
    AVIOContext* GetAVIO()
    {
        return m_pAvio;
    }
private:
    static int ReadCB(void* opaque, uint8_t* buf, int buf_size)
    {
        stOpaque* opa = (stOpaque*)opaque;
        
        int nRet = fread(buf, 1, buf_size, (FILE*)opa->ptr);
        if (nRet <= 0)
        {
            nRet = AVERROR_EOF;
        }
        else
        {
            opa->nPos += nRet;
        }
        return nRet;
    }
    static int WriteCB(void* opaque, uint8_t* buf, int buf_size)
    {
        stOpaque* opa = (stOpaque*)opaque;
        int nRet = fwrite(buf, 1, buf_size, (FILE*)opa->ptr);
        if (nRet <= 0)
        {
            nRet = AVERROR_EOF;
        }
        else
        {
            opa->nPos += nRet;
        }

        return nRet;
    }
    static int64_t SeekCB(void* opaque, int64_t offset, int whence)
    {
        int64_t nRet = 0;
        stOpaque* opa = (stOpaque*)opaque;

        switch (whence)
        {
        case AVSEEK_SIZE:
        {
            int64_t llFileSize = 0;

            if (opa->ptr)
            {
                int64_t llCurrentFilePosition = ftello64((FILE*)opa->ptr);

                if (fseeko64((FILE*)opa->ptr, 0, SEEK_END))
                {
                    return -1;
                }

                llFileSize = ftello64((FILE*)opa->ptr);

                if (fseeko64((FILE*)opa->ptr, llCurrentFilePosition, SEEK_SET))
                {
                    return -1;
                }
            }

            return llFileSize;
        }
        case SEEK_SET:
            nRet = fseeko64((FILE*)opa->ptr, offset, SEEK_SET);
            if (nRet >= 0)
            {
                opa->nPos = nRet;
                return opa->nPos;
            }
            else
            {
                return AVERROR_EOF;
            }
        case SEEK_CUR:
            nRet = fseeko64((FILE*)opa->ptr, offset, SEEK_CUR);
            if (nRet >= 0)
            {
                opa->nPos = nRet;
                return opa->nPos;
            }
            else
            {
                return AVERROR_EOF;
            }
        default: return AVERROR(EINVAL);
        }
        return 0;
    }

    struct stOpaque
    {
        void* ptr;
        int64_t nPos;
        int nWriteFlag;
    };
private:
    AVIOContext* m_pAvio;
    FILE* m_hFile;
    int m_nBufSize;

    stOpaque m_Opa;
};

//io_open��io_closeʵ��

class CVXAvIOInternal
{
public:

    struct stOpaque
    {
        void* ptr;
        int64_t nPos;
        int nWriteFlag;
    };

    CVXAvIOInternal()
        : m_pAvio(nullptr)
        , m_hFile(nullptr)
    {

    }
    ~CVXAvIOInternal()
    {
        if (m_pAvio)
        {
            av_free(m_pAvio->buffer);
            avio_context_free(&m_pAvio);

            m_pAvio = nullptr;
        }

        if (m_hFile)
        {
            fclose(m_hFile);
            m_hFile = nullptr;
        }

        m_Opa.nWriteFlag = 0;
    }

    int SetFileName(const char* lpFileName, bool bWrite)
    {
        if (m_hFile)
        {
            fclose(m_hFile);
            m_hFile = nullptr;
        }

        m_Opa.nWriteFlag = 0;
        m_Opa.nPos = 0;

        if (lpFileName == nullptr)
        {
            return 0;
        }

        m_hFile = fopen(lpFileName, "wb");
        if (m_hFile == nullptr)
        {
            return -1;
        }

        m_nBufSize = 2 * 1024 * 1024;

        if (bWrite)
        {
            m_Opa.nWriteFlag = 1;
        }
        else
        {
            m_Opa.nWriteFlag = 0;
        }

        m_Opa.ptr = this;

        m_pAvio = avio_alloc_context((unsigned char*)av_malloc(m_nBufSize), m_nBufSize, m_Opa.nWriteFlag, &m_Opa, this->ReadCB, this->WriteCB, this->SeekCB);
        if (m_pAvio == nullptr)
        {
            return -2;
        }

        return 1;
    }

    FILE* GetFileHandle()
    {
        return m_hFile;
    }

    void CloseHandle()
    {
        if (m_hFile)
        {
            fclose(m_hFile);
            m_hFile = nullptr;
        }
    }

    AVIOContext* GetAVIO()
    {
        return m_pAvio;
    }

private:
    static int ReadCB(void* opaque, uint8_t* buf, int buf_size)
    {
        stOpaque* opa = (stOpaque*)opaque;
        CVXAvIOInternal* pThis = (CVXAvIOInternal*)opa->ptr;
        int nRet = fread(buf, 1, buf_size, pThis->GetFileHandle());
        if (nRet <= 0)
        {
            nRet = AVERROR_EOF;
        }
        else
        {
            opa->nPos += nRet;
        }

        return nRet;
    }
    static int WriteCB(void* opaque, uint8_t* buf, int buf_size)
    {
        stOpaque* opa = (stOpaque*)opaque;
        CVXAvIOInternal* pThis = (CVXAvIOInternal*)opa->ptr;
        int nRet = fwrite(buf, 1, buf_size, pThis->GetFileHandle());
        if (nRet <= 0)
        {
            nRet = AVERROR_EOF;
        }
        else
        {
            opa->nPos += nRet;
        }

        return nRet;
    }
    static int64_t SeekCB(void* opaque, int64_t offset, int whence)
    {
        int64_t nRet = 0;
        stOpaque* opa = (stOpaque*)opaque;
        CVXAvIOInternal* pThis = (CVXAvIOInternal*)opa->ptr;
        switch (whence)
        {
        case AVSEEK_SIZE:
        {
            int64_t llFileSize = 0;

            if (pThis->GetFileHandle())
            {
                int64_t llCurrentFilePosition = ftello64(pThis->GetFileHandle());

                if (fseeko64(pThis->GetFileHandle(), 0, SEEK_END))
                {
                    return -1;
                }

                llFileSize = ftello64(pThis->GetFileHandle());

                if (fseeko64(pThis->GetFileHandle(), llCurrentFilePosition, SEEK_SET))
                {
                    return -1;
                }
            }

            return llFileSize;
        }
        case SEEK_SET:
            nRet = fseeko64(pThis->GetFileHandle(), offset, SEEK_SET);
            if (nRet >= 0)
            {
                opa->nPos = nRet;
                return opa->nPos;
            }
            else
            {
                return AVERROR_EOF;
            }
        case SEEK_CUR:
            nRet = fseeko64(pThis->GetFileHandle(), offset, SEEK_CUR);
            if (nRet >= 0)
            {
                opa->nPos = nRet;
                return opa->nPos;
            }
            else
            {
                return AVERROR_EOF;
            }
        default: return AVERROR(EINVAL);
        }
        return 0;
    }

private:
    AVIOContext* m_pAvio;
    FILE* m_hFile;
    int m_nBufSize;

    stOpaque m_Opa;
};

int VXIoOpen(AVFormatContext* s, AVIOContext** pb, const char* url, int flags, AVDictionary** options)
{
    int nRet = 0;
    bool bWrite = false;

    switch (flags)
    {
    case AVIO_FLAG_WRITE:
        bWrite = true;
        break;
    case AVIO_FLAG_READ_WRITE:
        bWrite = true;
        break;
    case AVIO_FLAG_READ:
        bWrite = false;
        break;
    }

    CVXAvIOInternal* pavio = new CVXAvIOInternal;

    nRet = pavio->SetFileName(url, bWrite);
    if (nRet <= 0)
    {
        if (pavio)
        {
            delete pavio;
            pavio = nullptr;
        }
        return AVERROR(EINVAL);
    }

    *pb = pavio->GetAVIO();

    return 0;
}

void VXIoClose(AVFormatContext* s, AVIOContext* pb)
{
    CVXAvIOInternal::stOpaque* opa = (CVXAvIOInternal::stOpaque*)pb->opaque;
    CVXAvIOInternal* pThis = (CVXAvIOInternal*)opa->ptr;

    pThis->CloseHandle();

    delete pThis;
}