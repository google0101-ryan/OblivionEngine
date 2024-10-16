#pragma once

#include <Framework/Entities/Entity.h>
#include <glm/glm.hpp>

// Literally just a container for the perspective/projection matrices
class Camera : public IEntityComponent
{
public:
    Camera();

    virtual void Update();
    /**
    * @brief Sets the camera to the active camera in the scene (the player's POV)
    */
    void SetActive() {isActiveCamera = true;}

    glm::mat4& GetView() { return view; }
    glm::mat4& GetProjection() { return projection; }
private:
    glm::mat4 view;
    glm::mat4 projection;
    bool isActiveCamera;
    float aspect;
    float fov;
};