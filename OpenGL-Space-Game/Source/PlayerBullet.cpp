#include "Headers/PlayerBullet.h"

PlayerBullet::PlayerBullet(Model* the_mesh, Shader* the_shader, PhysicsDraw* pPhys_draw) :
	Player(the_mesh, the_shader), m_pPhysicsDrawer(pPhys_draw)
{
	m_md2_model = dynamic_cast<MD2_Model*>(the_mesh);
	GetTransform().SetScale(glm::vec3(0.1f));

	SetAnimState(PlayerState::pIdle);
	m_cur_time = 0.0f;
	m_intval_time = 0.2f;
}

bool PlayerBullet::Update(float dt)
{
	BaseObject::Update(dt);

	HandleInput(dt);
	HandleAnimation(dt);
	UpdateCamera(dt);

	return true;
}

void PlayerBullet::HandleCollision(BaseObject*)
{
	printf("Player collided something!\n");
}

void PlayerBullet::HandleInput(const float dt)
{
	m_phys_obj->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION); // DON'T LET OUR PLAYER GO TO SLEEP, or we can't move it.

	static float motion = 0;
	static float upmotion = 0;
	static float LRDIR = 0;

	if (m_stabilize_ship_key)
	{
		m_phys_obj->SetPosition(btVector3(0, 0, 0));
		btQuaternion Rotation(0, 0, 0);
		m_phys_obj->SetOrientation(Rotation);
	}

	if (m_dock_key)
	{
		m_pPhysicsDrawer->ToggleDebugFlag(1 + 2);
	}

	// really bad hovercraft steering systme, gives us decent control and skid for a box though.
	if (m_up_key)
	{
		motion++;
		if (motion > 20) motion = 20;

	}
	else if (motion > 0) motion--;

	if (m_space_pressed)
	{
		if (upmotion < 10) upmotion = 40;
	}
	else if (upmotion > 0) upmotion--;

	if (m_down_key)
	{
		motion--;
		if (motion < -20) motion = -20;

	}
	else if (motion < 0) motion++;

	if (motion <= FLT_EPSILON && motion >= -FLT_EPSILON) {
		SetAnimState(PlayerState::pIdle);
		LRDIR = 0.0f; 
	}
	else
	{
		SetAnimState(PlayerState::pWalk);
	}

	LRDIR = 0;

	// some basic steering	
	if (m_left_key)		LRDIR = -0.2f;
	if (m_right_key)	LRDIR = 0.2f;

	m_phys_obj->GetRigidBody()->setDamping(0.2f, 0.15f);

	// if we are turning, increase the damping on the linear(forward) motion, if not tunring set the steer to 0.
	if ((m_left_key == 0 && m_right_key == 0))
		LRDIR = 0.0f;
	else	m_phys_obj->GetRigidBody()->setDamping(0.6f, 0.115f); // slightly reduce damping on angular

	// apply an impulse for forward motion and a simple Vel for steering.
	glm::vec3 forward = GetTransform().GetForward();
	btVector3 forwardbt = btVector3(forward.x, forward.y, forward.z);
	m_phys_obj->GetRigidBody()->applyCentralImpulse(forwardbt * motion * 30);
	m_phys_obj->GetRigidBody()->setAngularVelocity(btVector3(0, -1.0, 0) * LRDIR * 6);

}

void PlayerBullet::HandleAnimation(const float dt)
{
	if (m_md2_model == nullptr)
		return;

	if (m_md2_model->pObject.size() <= 0) return;

	m_cur_time += dt;
	if (m_cur_time > m_intval_time)
	{
		// clamp frames
		m_cur_frame++;
		if ((m_cur_frame < m_cur_anim->startFrame) || (m_cur_frame > m_cur_anim->endFrame - 1))
		{
			m_cur_frame = m_cur_anim->startFrame;
		}

		m_cur_time = 0.0f;
	}
}

void PlayerBullet::UpdateCamera(const float dt)
{
	cam_last_world_pos += (GetTransform().GetPosition() - cam_last_world_pos) * cam_pos_lerp_time;

	cam_last_world_look_at_pos += (GetTransform().GetPosition() - cam_last_world_look_at_pos) + cam_rot_lerp_time;

	cam_last_effect_pos += (glm::vec3((m_euler_force.y + (m_euler_force.z * 0.3f)) * 50,
		m_euler_force.x * -50, 0) - cam_last_effect_pos) * cam_effect_lerp_time;

	cam_last_up_dir += (GetTransform().GetUp() - cam_last_up_dir) * cam_up_dir_lerp_time;

	glm::vec3 rotated_cam_offset = glm::vec3((GetTransform().GetRotationMatrix() *
		glm::vec4(camera_offset + cam_last_effect_pos, 0)));
	glm::vec3 rotated_look_at_offset = glm::vec3((GetTransform().GetRotationMatrix() *
		glm::vec4(camera_look_at_offset + cam_last_effect_pos, 0)));

	m_active_camera->set_world_pos(cam_last_world_pos + rotated_cam_offset);
	m_active_camera->set_world_look_at_pos(cam_last_world_look_at_pos + rotated_look_at_offset);
	m_active_camera->set_up_dir(cam_last_up_dir);
}

bool PlayerBullet::Draw(const Camera& the_active_camera)
{
	UpdateUniforms(the_active_camera);

	if (m_md2_model == nullptr)
		return false;

	if (m_md2_model->pObject.size() <= 0) return false;
	
	t3DObject* pframe = &m_md2_model->pObject.at(m_cur_frame);

	glBindBuffer(GL_ARRAY_BUFFER, m_md2_model->vbo);
	glBindTexture(GL_TEXTURE_2D, m_md2_model->pMaterials.at(0).texureId);

	const void* mFirstTexCoord		= (const void*)(0);
	const void* mFirstPosition		= (const void*)(m_md2_model->texCoordDataStride + (m_cur_frame * m_md2_model->vertexDataStride));
	const void* mFirstnormpos		= (const void*)(m_md2_model->texCoordDataStride + (m_cur_frame * m_md2_model->vertexDataStride));
	const void* mFirstNextFramePos	= (const void*)(m_md2_model->texCoordDataStride + (m_cur_frame + 1 * m_md2_model->vertexDataStride));

	glEnableVertexAttribArray(this->positionLoc);
	glEnableVertexAttribArray(this->texCoordLoc);

	glVertexAttribPointer(this->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, mFirstPosition);
	glVertexAttribPointer(this->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, mFirstTexCoord);
	
	glDrawArrays(GL_TRIANGLES, 0, pframe->numOfFaces * 3);

	int error = 0;
	if ((error = glGetError()) != GL_NO_ERROR)
	{
		printf("PlayerBullet calls error%i\n", error);
	}

	return true;
}

void PlayerBullet::SetAnimState(PlayerState p_state)
{
	m_cur_anim_state = p_state;

	m_anim_change = true;

	switch (m_cur_anim_state)
	{
	case PlayerState::pIdle:
		m_cur_anim = GetAnimFromName("stand");
		break;
	case PlayerState::pWalk:
		m_cur_anim = GetAnimFromName("run");
		break;
	case PlayerState::pDeath:
		m_cur_anim = GetAnimFromName("death1");
		break;
	case PlayerState::pFalling:
		m_cur_anim = GetAnimFromName("flip");
		break;
	case PlayerState::pJump:
		m_cur_anim = GetAnimFromName("jump");
		break;
	case PlayerState::pPain:
		m_cur_anim = GetAnimFromName("pain1"); // pain1, pain2, pain3
		break;
	case PlayerState::pTaunt:
		m_cur_anim = GetAnimFromName("taunt");
		break;
	default:
		break;
	}
}

tAnimationInfo* PlayerBullet::GetAnimFromName(const std::string name)
{
	for (size_t i = 0; i < m_md2_model->pAnimations.size(); i++)
	{
		tAnimationInfo* pAnim = &m_md2_model->pAnimations.at(i);
		if (name == pAnim->strName)
			return pAnim;
	}
}

void PlayerBullet::UpdateUniforms(const Camera& the_active_camera)
{
	m_shader->use();

	// make the MVP
	m_shader->set_vec4("ambient", { 1.0f, 1.0f, 1.0f, 1.0f });

	const glm::mat4 mvp = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4() * GetTransform().GetModelMatrix(); // Remember order seems backwards
	m_shader->set_mat4("MVP", mvp);

	m_shader->set_mat4("M", GetTransform().GetModelMatrix());

	const glm::mat4 normal_mat = glm::transpose(glm::inverse(GetTransform().GetModelMatrix()));
	m_shader->set_mat4("normal_matrix", normal_mat);
}