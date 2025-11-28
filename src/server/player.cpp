#include "player.h"
#include "iengine.h"
#include "physics_world.h"
#include "debugrender.h"
#include <imgui.h>

REGISTER_ENTITY(Player, "player");

Player::Player()
{
	m_userCmd = {};
	m_Direction = glm::vec3(0.0f);
}

Player::~Player()
{
}

void Player::Think()
{	
	// #TODO: !!!
	float dt = GetEngine()->GetDeltaTime();

	// movement
	if (m_userCmd.walkForward)
		m_Origin += m_Direction * 5.5f * dt;
	if (m_userCmd.walkBackward)
		m_Origin -= m_Direction * 5.5f * dt;
	if (m_userCmd.strafeLeft)
		m_Origin -= glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)) ) * 5.0f * dt;
	if (m_userCmd.strafeRight)
		m_Origin += glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)) ) * 5.0f * dt;
}

void Player::ParseUserCmd(const UserCmd* pUserCmd)
{
	if (!pUserCmd)
		return;

	m_userCmd = *pUserCmd;

	//if (pUserCmd->action)
	//	m_RayPickMode = true;
	//else
	//	m_RayPickMode = false;

	m_Direction = glm::vec3(pUserCmd->dirx, pUserCmd->diry, pUserCmd->dirz);
	m_Direction = glm::normalize(m_Direction);
}
