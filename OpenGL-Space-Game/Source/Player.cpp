#include "Headers/Player.h"

Player::Player(Model* the_mesh, Shader* the_shader)
	: DrawableObject(the_shader)
{
	m_model = the_mesh;
}

void Player::Init(Camera* the_cam)
{
	m_active_camera = the_cam;
	m_ambient_colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}