#pragma once

#include <appsystem/AppSystem.h>
#include <vector>

abstract_class IFile
{
public:
    virtual void seek(int offs, int whence) {}
    virtual size_t tell() { return 0; }
    virtual int read(void* buf, size_t size) { return 0; }
};

#define FILESYSTEM_INTERFACE_NAME "FileSysV001"
abstract_class IFileSystem : public IAppSystem
{
public:
    virtual IFile* OpenFileRead( const char* pName ) = 0;
    virtual IFile* OpenFileWrite( const char* pName ) = 0;
    virtual void CloseFile( IFile* pFile ) = 0;
    virtual bool ListDirectory(const char* directory, std::vector<std::string>& list, const char* ext = nullptr) = 0;
};