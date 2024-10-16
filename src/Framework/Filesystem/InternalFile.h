#pragma once

#include "File.h"
#include <cstring>

enum RegularFileMode
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_READWRITE
};

class RegularFile : public IFile
{
public:
    RegularFile(const char* szPath, RegularFileMode mode)
    {
        char* modeStr;
        switch (mode)
        {
        case FILE_MODE_READ:
            modeStr = "rb";
        case FILE_MODE_WRITE:
            modeStr = "wb";
        case FILE_MODE_READWRITE:
            modeStr = "rw";
        }

        f = fopen(szPath, modeStr);
    }

    virtual void Close()
    {
        fclose(f);
    }

    virtual int Read(int len, void* buf)
    {
        len = fread(buf, 1, len, f);
        *(&((char*)buf)[len]) = 0;
        return len;
    }

    virtual void Write(int len, void* buf)
    {
        fwrite(buf, 1, len, f);
    }

    virtual void Seek(int whence, int offs)
    {
        fseek(f, offs, whence);
    }

    virtual int Tell()
    {
        return ftell(f);
    }
private:
    FILE* f;
};

class MemoryFile : public IFile
{
public:
    MemoryFile(unsigned char* pData, long iDataLen)
    {
        m_iDataLen = iDataLen;
        m_pData = pData;
    }

    virtual void Close()
    {
    }

    virtual int Read(int len, void* buf)
    {
        if (len+m_iOffset >= m_iDataLen)
        {
            len = m_iDataLen - m_iOffset;
        }
        memcpy(buf, m_pData+m_iOffset, len);
        return len;
    }

    virtual void Write(int len, void* buf)
    {
        if (len+m_iOffset >= m_iDataLen)
        {
            len = m_iDataLen - m_iOffset;
        }
        memcpy(m_pData+m_iOffset, buf, len);
    }

    virtual void Seek(int whence, int offs)
    {
        switch (whence)
        {
        case SEEK_SET:
            m_iOffset = offs;
            break;
        case SEEK_CUR:
            m_iOffset += offs;
            break;
        case SEEK_END:
            m_iOffset = m_iDataLen - offs;
            break;
        }
    }

    virtual int Tell()
    {
        return m_iOffset;
    }
private:
    unsigned char* m_pData;
    long m_iOffset;
    long m_iDataLen;
};