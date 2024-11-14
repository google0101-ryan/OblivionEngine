#pragma once

#include <layer2/FileSystem.h>

class CRegularFile : public IFile
{
public:
    CRegularFile( const char* pPath, bool isRead )
    {
        m_pFile = fopen(pPath, isRead ? "rb" : "w");
    }
private:
    FILE* m_pFile;
};