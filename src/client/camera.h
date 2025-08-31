#ifndef CAMERA_H
#define CAMERA_H

#include "render_shared.h"

enum CameraProjection_t
{
	ORTHOGONAL,
	PERSPECTIVE,
};

class Camera
{
public:
	Camera();
	~Camera();

	inline void LookAt(const glm::vec3& direction) { m_direction = direction /*- m_position*/; }

	inline void SetProjection(CameraProjection_t projection) { m_projectionType = projection; }
	inline CameraProjection_t GetProjection() const { return m_projectionType; }

	// In degrees

	inline void			SetFOV		(float fov)						{ m_fov = fov; }
	inline float		GetFOV		() const						{ return m_fov; }
	inline void			SetPosition	(const glm::vec3& position)		{ m_position = position; }
	inline glm::vec3	GetPosition	() const						{ return m_position; }

	void SetYawPitch(float yaw, float pitch);

	inline glm::vec3 GetDirection	() const { return m_direction; }
	inline glm::vec3 GetUp			() const { return m_up; }

	// FIXME: Control zNear/zFar in perspective projection
	glm::mat4 GetProjectionMatrix	(const Viewport& viewport) const;
	glm::mat4 GetViewMatrix			() const;
	glm::mat4 GetTranslationMatrix	() const;

private:
	CameraProjection_t m_projectionType;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_up;

	float m_yaw;
	float m_pitch;

	float m_fov;
};

#endif // !CAMERA_H
