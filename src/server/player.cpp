#include "player.h"
#include "iengine.h"
#include <imgui.h>

REGISTER_ENTITY(Player, "player");

Player::Player()
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

	// calculate yaw and pitch
	static float s_sensitivity = 0.14f;
	static float yaw = 0.0f, pitch = 0.0f;
	yaw += (float)pUserCmd->mouseX * s_sensitivity;
	pitch += (float)-pUserCmd->mouseY * s_sensitivity;

	if (pitch >= 89.0f)
		pitch = 89.0f;
	else if (pitch <= -89.0f)
		pitch = -89.0f;

	// calculate direction of looking
	glm::vec3 direction = glm::vec3(0.0f);
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	m_Orient = glm::quat(direction);
	m_Orient = glm::normalize(m_Orient);

	// #TODO: !!!
	float dt = GetEngine()->GetDeltaTime();

	// movement
	if (pUserCmd->walkForward)
		m_Origin += direction * 1.5f * dt;
	if (pUserCmd->walkBackward)
		m_Origin -= direction * 1.5f * dt;
	if (pUserCmd->strafeLeft)
		m_Origin -= glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * (10.0f * dt);
	if (pUserCmd->strafeRight)
		m_Origin += glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f))) * (10.0f * dt);
}
