#include "Headers/Camera.h"

Camera::Camera(glm::vec3 world_pos, glm::vec3 look_at_world_pos, glm::vec3 up_dir) :
	m_world_pos(world_pos), m_world_look_at_pos(look_at_world_pos), m_up_dir(up_dir)
{
	m_projection	= glm::mat4(1.0f);
	m_view			= glm::mat4(1.0f);
}

void Camera::Init()
{
	update_projection();
	update_view();
}

void Camera::update_projection()
{
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : near 0.1 unit <->  far 100 units
	m_projection = glm::perspective(m_fov, m_ratio, m_near_plane, m_far_plane);
}

void Camera::update_view()
{
	m_view = glm::lookAt(
		glm::vec3(m_world_pos), // the cam pos in world
		glm::vec3(m_world_look_at_pos), // look at a world pos
		glm::vec3(m_up_dir) // pointing up is 0,1,0 ( 0,-1,0 will be upside-down));
	);
}
