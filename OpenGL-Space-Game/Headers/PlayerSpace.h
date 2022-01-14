#pragma once
#include "Player.h"
#include "SpaceStation.h"
class PlayerSpace :
	public Player
{
protected:
	float total_time = 0.0f;

	float m_mass = 10000.0f;

	glm::vec3 m_physics_forward_dir = glm::vec3(0, 0, 0);

	float m_forward_accel = 0.5f;
	float m_forward_accel_big_boost = 1.5f;
	float m_forward_accel_small_boost = 0.75f;
	float m_backwards_force_limit = 1.0f;

	glm::vec3 m_euler_accel = glm::vec3(0.7f, 0.7f, 0.1f);
	glm::vec3 m_euler_deaccel = glm::vec3(0.5f, 0.5f, 0.1f);
	glm::vec3 m_euler_rest_boundary = glm::vec3(0.01f, 0.01f, 0.0f);

	// camera player behaviour vars
	glm::vec3 cam_last_world_pos			= glm::vec3(0.0f);
	glm::vec3 cam_last_world_look_at_pos	= glm::vec3(0.0f);
	glm::vec3 cam_last_effect_pos			= glm::vec3(0.0f);
	glm::vec3 cam_last_up_dir				= glm::vec3(0.0f);

	glm::vec3 camera_offset = glm::vec3(0.0f, 4.0f, -10.0f);
	glm::vec3 camera_look_at_offset = glm::vec3(0.0f, 0.5f, 3.0f);

	float cam_pos_lerp_time = 0.6f;
	float cam_rot_lerp_time = 0.1f;
	float cam_effect_lerp_time = 0.1f;
	float cam_up_dir_lerp_time = 0.5f;

	float roll_cam_sway_multiplier = 0.3f;

	// dock
	glm::quat last_orient;
	glm::vec3 last_dock_pos;

	MeshModel* m_mesh_model;

public:
	PlayerSpace(Model* the_mesh, Shader* the_shader);
	SpaceStation* m_pSpace_station;
	glm::vec3 m_dock_pos;
	bool m_first_call = false;
	virtual void Init(Camera*);

	virtual bool Update(float dt); // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera); // and a draw
	virtual void UpdateUniforms(const Camera& the_active_camera);

	void UpdateDock(float dt);
	float DistFromDock();
	void HandleShipInput(const float dt);
	void UpdateShipCamera(const float dt);

	char m_the_one_coll[4]	{' '};
	int m_the_one_total		= 0;
};

