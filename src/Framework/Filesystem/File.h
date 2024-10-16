#pragma once

#include <cstdio>

enum FileSeekDir
{
    FILE_SEEK_SET,
    FILE_SEEK_CUR,
    FILE_SEEK_END
};

class IFile
{
public:
    virtual void Close() = 0;
    virtual int Read(int len, void* buf) = 0;
    virtual void Write(int len, void* buf) = 0;
    virtual void Seek(int whence, int offs) = 0;
    virtual int Tell() = 0;
};