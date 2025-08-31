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
	//static int lastMouseX = 0, lastMouseY = 0;

	//int deltaX = pUserCmd->mouseX - lastMouseX;
	//int deltaY = pUserCmd->mouseY - lastMouseY;

	//lastMouseX = pUserCmd->mouseX;
	//lastMouseY = pUserCmd->mouseY;

	static float yaw = 0.0f, pitch = 0.0f;

	//yaw += (float)-deltaX * s_sensitivity;
	//pitch += (float)-deltaY * s_sensitivity;

	yaw += (float)-pUserCmd->deltaX * s_sensitivity;
	pitch += (float)-pUserCmd->deltaY * s_sensitivity;


	if (pitch >= 89.0f)
		pitch = 89.0f;
	else if (pitch <= -89.0f)
		pitch = -89.0f;

	glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	m_Orient = glm::normalize(qYaw * qPitch);

	glm::vec3 forward = m_Orient * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

	// #TODO: !!!
	float dt = GetEngine()->GetDeltaTime();

	// movement
	if (pUserCmd->walkForward)
		m_Origin += forward * 10.5f * dt;
	if (pUserCmd->walkBackward)
		m_Origin -= forward * 10.5f * dt;
	if (pUserCmd->strafeLeft)
		m_Origin -= right * 10.0f * dt;
	if (pUserCmd->strafeRight)
		m_Origin += right * 10.0f * dt;
}
