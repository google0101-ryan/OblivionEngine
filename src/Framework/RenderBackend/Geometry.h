#pragma once

#include <Framework/RenderBackend/RenderGraphImage.h>
#include <glm/glm.hpp>

typedef uint64_t vertCacheHandle_t;

struct DrawVert
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoords;
};

struct Geometry
{
    int numVerts;
    DrawVert* verts;
    int numIndices;
    uint16_t* indices;
    vertCacheHandle_t vertCacheHandle, indexCacheHandle;
    uint64_t dynamicUBOOffset;
    IRenderImage* m_Texture;
    glm::mat4 model;
};

struct UBO
{
    glm::mat4 projection;
    glm::mat4 view;
};

class Model
{
public:
    Model();
    void SetTexture(std::string texName);
    void UpdateModel(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

    Geometry* GetGeo();
private:
    void MakeDefault();

    Geometry* m_Geo;
};

extern Geometry triangle;
extern Geometry rectangle;
extern Geometry defaultCube;
extern Model* defaultModel;