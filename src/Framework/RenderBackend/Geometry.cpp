#include <Framework/RenderBackend/RenderGraphImage.h>
#include <Framework/RenderBackend/Geometry.h>

Geometry triangle;
Geometry rectangle;
Geometry defaultCube;

Model::Model()
{
    MakeDefault();
}

void Model::MakeDefault()
{
    m_Geo = &defaultCube;
}

void Model::SetTexture(std::string name)
{
    m_Geo->m_Texture = CreateImage(name);
}

Geometry* Model::GetGeo()
{
    return m_Geo;
}