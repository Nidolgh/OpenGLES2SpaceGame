#pragma once
#include "Planet.h"

class BulletObject
	: public Planet
{
protected:
	bool m_static = false;

public:
	BulletObject(Model* the_mesh, Shader* the_shader);

	virtual bool Update(float dt); // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera); // and a draw
	virtual void UpdateUniforms(const Camera& the_active_camera);

	virtual void HandleCollision(BaseObject*) { printf("BulletObject collided.\n"); }
};

