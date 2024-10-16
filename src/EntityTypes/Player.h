#pragma  once

#include <Framework/Entities/Entity.h>
#include <Framework/Entities/Components/Camera.h>

class Player : Entity
{
public:
    Player();

    virtual void Update() override;
private:
    Camera m_PlayerView;
};