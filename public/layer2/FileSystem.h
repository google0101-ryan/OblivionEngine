#pragma once

#include <appsystem/AppSystem.h>

abstract_class IFile
{
};

#define FILESYSTEM_INTERFACE_NAME "FileSysV001"
abstract_class IFileSystem : public IAppSystem
{
public:
    virtual IFile* OpenFileRead( const char* pName ) = 0;
    virtual IFile* OpenFileWrite( const char* pName ) = 0;
    virtual void CloseFile( IFile* pFile ) = 0;
};