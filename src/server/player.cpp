#include "player.h"
#include "iengine.h"
#include "physics_world.h"
#include "debugrender.h"
#include <imgui.h>

REGISTER_ENTITY(Player, "player");

Player::Player() :
	m_RayPickMode(false)
{
}

Player::~Player()
{
}

void Player::Think()
{
}

void Player::ParseUserCmd(const UserCmd* pUserCmd)
{
	if (!pUserCmd)
		return;

	if (pUserCmd->action)
		m_RayPickMode = true;
	else
		m_RayPickMode = false;

	m_RayOrigin = glm::vec3(pUserCmd->posx, pUserCmd->posy, pUserCmd->posz);
	m_Direction = glm::vec3(pUserCmd->dirx, pUserCmd->diry, pUserCmd->dirz);

	glm::vec3 direction = glm::vec3(pUserCmd->dirx, pUserCmd->diry, pUserCmd->dirz);

	// #TODO: !!!
	float dt = GetEngine()->GetDeltaTime();

	// movement
	if (pUserCmd->walkForward)
		m_Origin += direction * 100.5f * dt;
	if (pUserCmd->walkBackward)
		m_Origin -= direction * 100.5f * dt;
	if (pUserCmd->strafeLeft)
		m_Origin -= glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)) * 100.0f * dt;
	if (pUserCmd->strafeRight)
		m_Origin += glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)) * 100.0f * dt;
}
