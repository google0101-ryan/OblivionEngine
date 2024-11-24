#pragma once

#include <layer2/FileSystem.h>

class CRegularFile : public IFile
{
public:
    CRegularFile( const char* pPath, bool isRead )
    {
        m_pFile = fopen(pPath, isRead ? "rb" : "w");
    }

    virtual void seek(int offs, int whence)
    {
        fseek(m_pFile, offs, whence);
    }

    virtual size_t tell()
    {
        return ftell(m_pFile);
    }

    virtual int read(char* buf, size_t size)
    {
        return fread(buf, size, 1, m_pFile);
    }
private:
    FILE* m_pFile;
};