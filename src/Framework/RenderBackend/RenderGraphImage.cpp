#include "RenderGraphImage.h"

extern IImageData* CreateAPIData(IRenderImage*);

IRenderImage::IRenderImage(std::string name, bool createAPIData)
: IResource(RT_IMAGE, name)
{
    if (createAPIData)
    {
        m_apiData = CreateAPIData(this);
        Update();
    }
}

void IRenderImage::Update()
{
    m_iWidth = m_apiData->GetWidth();
    m_iHeight = m_apiData->GetHeight();
    m_Format = m_apiData->GetFormat();
}
