#pragma  once

#include <Framework/Entities/Entity.h>
#include <Framework/RenderBackend/Geometry.h>

class Renderable : public IEntityComponent
{
public:
    Renderable();

    virtual void Update() override;

    void SetAlbedoMap(std::string textureName);
private:
    Model* m_Model;
};