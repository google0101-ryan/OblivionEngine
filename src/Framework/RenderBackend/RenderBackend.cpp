#include "Framework/RenderBackend/RenderGraphImage.h"
#include <Framework/RenderBackend/RenderBackend.h>
#include <Framework/RenderBackend/Geometry.h>

#include <Util/Logger.h>
#include <cstdint>

void IRenderBackend::SendCommand(int iCommand, void *pData)
{
    RenderCmdData data;
    data.m_iCmd = iCommand;
    data.m_pData = pData;

    m_RenderCmdQ.push(data);
}

glm::vec3 cubeColors[] =
{
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),  
};

void AddCubeFace(Geometry* g, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4)
{
    g->verts[g->numVerts+0].pos = v1;
    g->verts[g->numVerts+1].pos = v2;
    g->verts[g->numVerts+2].pos = v3;
    g->verts[g->numVerts+3].pos = v4;

    g->verts[g->numVerts+0].color = cubeColors[0];
    g->verts[g->numVerts+1].color = cubeColors[1];
    g->verts[g->numVerts+2].color = cubeColors[2];
    g->verts[g->numVerts+3].color = cubeColors[3];

    g->verts[g->numVerts+0].texCoords = glm::vec2(0, 0);
    g->verts[g->numVerts+1].texCoords = glm::vec2(1, 0);
    g->verts[g->numVerts+2].texCoords = glm::vec2(1, 1);
    g->verts[g->numVerts+3].texCoords = glm::vec2(0, 1);

    g->verts[g->numVerts+0].texCoords = glm::vec2(0, 0);
    g->verts[g->numVerts+1].texCoords = glm::vec2(1, 0);
    g->verts[g->numVerts+2].texCoords = glm::vec2(1, 1);
    g->verts[g->numVerts+3].texCoords = glm::vec2(0, 1);

    g->indices[g->numIndices+0] = g->numVerts+0;
    g->indices[g->numIndices+1] = g->numVerts+1;
    g->indices[g->numIndices+2] = g->numVerts+2;
    g->indices[g->numIndices+3] = g->numVerts+0;
    g->indices[g->numIndices+4] = g->numVerts+2;
    g->indices[g->numIndices+5] = g->numVerts+3;

    g->numVerts += 4;
    g->numIndices += 6;
}

Model* defaultModel;

static void InitStaticGeometry()
{
    triangle.numVerts = 3;
    triangle.numIndices = 3;
    triangle.verts = new DrawVert[3];
    triangle.indices = new uint16_t[3];

    triangle.verts[0].pos = glm::vec3(0.0f, -0.5f, 0.0f);
    triangle.verts[1].pos = glm::vec3(0.5f, 0.5f, 0.0f);
    triangle.verts[2].pos = glm::vec3(-0.5f, 0.5f, 0.0f);

    triangle.verts[0].color = glm::vec3(1.0f, 0.0f, 0.0f);
    triangle.verts[1].color = glm::vec3(0.0f, 1.0f, 0.0f);
    triangle.verts[2].color = glm::vec3(0.0f, 0.0f, 1.0f);
    triangle.indices[0] = 0;
    triangle.indices[1] = 1;
    triangle.indices[2] = 2;

    rectangle.numVerts = 4;
    rectangle.verts = new DrawVert[4];
    
    /*const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};*/

    rectangle.verts[0].pos = glm::vec3(-1.0f, -1.0f, 0.0f)*1.5f;
    rectangle.verts[1].pos = glm::vec3(1.0f, -1.0f, 0.0f)*1.5f;
    rectangle.verts[2].pos = glm::vec3(1.0f, 1.0f, 0.0f)*1.5f;
    rectangle.verts[3].pos = glm::vec3(-1.0f, 1.0f, 0.0f)*1.5f;

    rectangle.verts[0].color = glm::vec3(1.0f, 0.0f, 0.0f);
    rectangle.verts[1].color = glm::vec3(0.0f, 1.0f, 0.0f);
    rectangle.verts[2].color = glm::vec3(0.0f, 0.0f, 1.0f);
    rectangle.verts[3].color = glm::vec3(1.0f, 1.0f, 1.0f);

    rectangle.verts[0].texCoords = glm::vec2(0, 0);
    rectangle.verts[1].texCoords = glm::vec2(1, 0);
    rectangle.verts[2].texCoords = glm::vec2(1, 1);
    rectangle.verts[3].texCoords = glm::vec2(0, 1);

    rectangle.numIndices = 6;
    rectangle.indices = new uint16_t[6];
    rectangle.indices[0] = 0;
    rectangle.indices[1] = 1;
    rectangle.indices[2] = 2;
    rectangle.indices[3] = 2;
    rectangle.indices[4] = 3;
    rectangle.indices[5] = 0;

    defaultCube.numIndices = 0;
    defaultCube.indices = new uint16_t[36];
    defaultCube.numVerts = 0;
    defaultCube.verts = new DrawVert[24];
    
    AddCubeFace(&defaultCube, glm::vec3(-1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1) );
	AddCubeFace(&defaultCube, glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, -1) );

	AddCubeFace(&defaultCube, glm::vec3(1, -1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1) );
	AddCubeFace(&defaultCube, glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1) );

	AddCubeFace(&defaultCube, glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1), glm::vec3(1, -1, -1), glm::vec3(-1, -1, -1) );
	AddCubeFace(&defaultCube, glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1, 1, 1) );
    defaultCube.m_Texture = CreateImage("default.jpg");

    defaultModel = new Model();
}

void IRenderBackend::DoWork()
{
    if (m_bExitRequested)
        return;

    auto cmd = m_RenderCmdQ.pop();

    switch (cmd.m_iCmd)
    {
    case RENDER_CMD_INIT:
        SetName("Backend");
        Init((RenderInitArgs*)cmd.m_pData);
        InitStaticGeometry();
        break;
    case RENDER_CMD_BEGIN_FRAME:
        BeginFrame();
        break;
    case RENDER_CMD_SUBMIT_MESH:
        SubmitGeometry((Geometry*)cmd.m_pData);
        break;
    case RENDER_CMD_DRAWFRAME:
        DrawFrame(cmd.m_pData);
        break;
    default:
        Logger::Log(LogLevel::FATAL, "Unknown command %d sent to rendering backend\n");
    }
    m_BackendDoneCond.notify_all();
}

void IRenderBackend::Shutdown()
{
    printf("IRenderBackend::Shutdown()\n");
    PlatShutdown();
}
