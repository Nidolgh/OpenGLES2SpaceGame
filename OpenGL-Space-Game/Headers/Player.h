#pragma once
#include "DrawableObject.h"
#include "Utils/TriangleCollection/MeshModel.h"

class Player : public DrawableObject
{
protected:
	Model* m_model; // todo make animated

	float m_forward_force_limit			= 2.5f;
	glm::vec3 m_euler_force_max_limit	= glm::vec3(0.05f, 0.05f, 0.05f);

	float m_forward_force	= 0.0f;
	glm::vec3 m_euler_force = glm::vec3(0.0f, 0.0f, 0.0f);

public:
	Player(Model* the_mesh, Shader* the_shader);

	virtual void Init(Camera*);
	
	virtual bool Update(float dt) = 0; // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera) = 0; // and a draw
	virtual void UpdateUniforms(const Camera& the_active_camera) = 0;

	virtual void HandleCollision(BaseObject*) { printf("Player collided.\n"); }

	float GetForwardForce()			const { return m_forward_force; }
	float GetForwardForceLimit()	const { return m_forward_force_limit; }
	glm::vec3 GetEulerForce()		const { return m_euler_force; }
	glm::vec3 GetEulerForceLimit()	const { return m_euler_force_max_limit; }

	Model& GetMeshModel() { return *m_model; }

	// Camera
	Camera* m_active_camera;
	
	// input
	bool m_left_key				= false;
	bool m_right_key			= false;
	bool m_up_key				= false;
	bool m_down_key				= false;

	// space input
	bool m_dock_key				= false;
	bool m_fire_key				= false;
	bool m_stabilize_ship_key	= false;
	bool m_boost_key			= false;
	
	// directional
	float m_hor_mouse_vel = 0.0f;
	float m_ver_mouse_vel = 0.0f;

	bool m_space_pressed		= false;

	// debug
	bool m_set_forward_key		= false;
};