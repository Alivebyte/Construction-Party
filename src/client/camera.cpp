#include "ientity.h"
#include "camera.h"

Camera::Camera()
{
	m_projectionType = PERSPECTIVE;
	m_position = glm::vec3(0.0f);
	m_direction = glm::vec3(0.0f, 0.0f, 1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fov = 70.0f;
	m_yaw = 0.0f;
	m_pitch = 0.0f;
}

Camera::~Camera()
{
}

void Camera::SetYawPitch(float yaw, float pitch)
{
	m_yaw = yaw;
	m_pitch = pitch;

	m_direction = glm::vec3(0.0f);
	m_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_direction.y = sin(glm::radians(pitch));
	m_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
}

glm::mat4 Camera::GetProjectionMatrix(const Viewport& viewport) const
{
	glm::mat4 proj = glm::mat4(1.0f);
	if (m_projectionType == ORTHOGONAL)
	{
		proj = glm::ortho(0.0f, (float)viewport.width, 0.0f, (float)viewport.height, 0.1f, 1000.0f);
	}
	else // if (m_projectionType == PERSPECTIVE): Control return in all path's
	{
		float aspectRatio = (float)viewport.width / (float)viewport.height;
		proj = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
	}

	return proj;
}

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat4 view = glm::mat4(1.0f);

	glm::vec3 cameraFront = glm::normalize(m_direction);

	view = glm::lookAt(m_position, m_position + cameraFront, m_up);
	return view;
}

glm::mat4 Camera::GetTranslationMatrix() const
{
	glm::mat4 t = glm::mat4(1.0f);
	t = glm::translate(t, m_position);
	t = glm::rotate(t, m_direction.x, glm::vec3(1.0f, 0.0f, 0.0f));
	t = glm::rotate(t, m_direction.y, glm::vec3(0.0f, 1.0f, 0.0f));
	t = glm::rotate(t, m_direction.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return t;
}
