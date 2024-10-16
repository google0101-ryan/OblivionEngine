#include <EntityTypes/Player.h>

Player::Player()
: Entity("player")
{
    AddComponent("camera", &m_PlayerView);
    m_PlayerView.SetActive();
}

void Player::Update()
{
    // TODO: Process w, a, s, d, etc.

    BaseClass::Update();
}