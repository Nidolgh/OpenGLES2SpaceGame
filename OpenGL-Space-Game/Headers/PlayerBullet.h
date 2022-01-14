#pragma once
#include "Headers/Player.h"
#include <Utils/3DModelLoading/MD2/MD2_Model.h>
class PlayerBullet :
	public Player
{
protected:
	MD2_Model* m_md2_model;

	enum PlayerState {
		pIdle,
		pWalk,
		pDeath,
		pFalling,
		pJump,
		pTaunt,
		pPain
	};
	PlayerState m_cur_anim_state;

	PhysicsDraw* m_pPhysicsDrawer;

	// todo move to camera class?
	glm::vec3 cam_last_world_pos = glm::vec3(0.0f);
	glm::vec3 cam_last_world_look_at_pos = glm::vec3(0.0f);
	glm::vec3 cam_last_effect_pos = glm::vec3(0.0f);
	glm::vec3 cam_last_up_dir = glm::vec3(0.0f);

	glm::vec3 camera_offset = glm::vec3(0.0f, 4.0f, -10.0f);
	glm::vec3 camera_look_at_offset = glm::vec3(0.0f, 0.5f, 3.0f);

	float cam_pos_lerp_time = 0.6f;
	float cam_rot_lerp_time = 0.1f;
	float cam_effect_lerp_time = 0.1f;
	float cam_up_dir_lerp_time = 0.5f;

	// animation 
	tAnimationInfo* m_cur_anim;
	int				m_cur_frame = 0;
	float			m_cur_time;
	float			m_intval_time;
	bool			m_anim_change;

	void SetAnimState(PlayerState p_state);
	tAnimationInfo* GetAnimFromName(const std::string name);
	
public:
	PlayerBullet(Model* the_mesh, Shader* the_shader, PhysicsDraw*);

	virtual bool Update(float dt); // we supply an update system, it can be an all in one system
	virtual bool Draw(const Camera& the_active_camera); // and a draw
	virtual void UpdateUniforms(const Camera& the_active_camera);

	virtual void HandleCollision(BaseObject*);
	
	void HandleInput(const float dt);
	void HandleAnimation(const float dt);

	void UpdateCamera(const float dt);
};