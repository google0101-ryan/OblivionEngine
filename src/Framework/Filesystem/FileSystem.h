#pragma once

#include "File.h"

class IFileSystem
{
public:
    virtual void Initialize() = 0;
    virtual IFile* OpenFileRead(const char* fileName) = 0;
    virtual void CloseFile(IFile* file);
};

extern IFileSystem* g_pFilesystem;