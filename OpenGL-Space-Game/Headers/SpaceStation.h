#pragma once
#include "Headers/Planet.h"

class SpaceStation :
	public Planet
{
protected:
	//glm::vec3 m_local_dock_position = glm::vec3(-50.0f, -5.0f, 18.0f);
	glm::vec3 m_local_dock_position;

public:
	SpaceStation(Model* the_mesh, Shader* the_shader);
	virtual ~SpaceStation();
	bool Init(const bool is_shaded);

	virtual bool Update(float dt); // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera); // and a draw

	virtual void UpdateUniforms(const Camera& the_active_camera);

	glm::vec3 GetLocalDockPosition();

	bool m_is_dockable = false;
};