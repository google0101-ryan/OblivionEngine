#pragma once

#include <Framework/RenderBackend/RenderGraphResource.h>
#include <Framework/RenderBackend/RenderFormat.h>

class IImageData
{
public:
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual ColorFormat GetFormat() const = 0;
};

// Common image data

class IRenderImage : public IResource
{
public:
    IRenderImage(std::string name, bool bCreateAPIData = true);
    void Update();

    IImageData* GetAPIData() const {return m_apiData;}
    void SetAPIData(IImageData* pData) { m_apiData = pData; Update(); }

    int GetWidth() const { return m_iWidth; }
    int GetHeight() const { return m_iHeight; }
    ColorFormat GetFormat() const {return m_Format;}
private:
    int m_iWidth, m_iHeight;
    ColorFormat m_Format;
    IImageData* m_apiData; // API-specific
};

IRenderImage* CreateImage(std::string path);