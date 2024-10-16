#include <Framework/RenderBackend/RenderGraphImage.h>
#include <Framework/RenderBackend/Geometry.h>
#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

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

void Model::UpdateModel(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
    glm::mat4 identity = glm::mat4(1.0f);
    model = glm::translate(identity, pos);
    model = glm::rotate(model, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
}

Geometry* Model::GetGeo()
{
    return m_Geo;
}