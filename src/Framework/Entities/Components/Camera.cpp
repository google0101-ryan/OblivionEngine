#include <Framework/Engine/Engine.h>
#include <Framework/GameWorld/GameWorld.h>
#include <Framework/Entities/Components/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    aspect = (float)g_pEngine->GetMainWindow()->GetWidth() / g_pEngine->GetMainWindow()->GetHeight();
    fov = 45.0f;
    isActiveCamera = false;
}

void Camera::Update()
{
    projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    view = glm::lookAt(m_pParent->pos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if (isActiveCamera)
    {
        g_GameWorld->SetCamera(this);
    }
}