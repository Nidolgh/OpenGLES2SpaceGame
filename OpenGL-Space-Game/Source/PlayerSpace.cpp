#include "Headers/PlayerSpace.h"

PlayerSpace::PlayerSpace(Model* the_mesh, Shader* the_shader) :
	Player(the_mesh, the_shader)
{
	m_mesh_model = dynamic_cast<MeshModel*>(the_mesh);
}

void PlayerSpace::Init(Camera* the_cam)
{
	Player::Init(the_cam);

	// set cam values
	last_dock_pos = cam_last_world_pos = cam_last_world_look_at_pos = GetTransform().GetPosition();
	cam_last_up_dir = GetTransform().GetUp();

	GetTransform().m_space_movement = true;
}

bool PlayerSpace::Update(float dt)
{
	total_time += dt;

	HandleShipInput(dt);
	UpdateShipCamera(dt);

	return true;
}

bool PlayerSpace::Draw(const Camera& the_active_camera)
{
	// only update
	UpdateUniforms(the_active_camera);

	for (size_t i = 0; i < m_mesh_model->m_drawObjects.size(); i++)
	{
		MeshModel::DrawObject o = m_mesh_model->m_drawObjects[i];

		m_shader->use();
		// Use the program object
		glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);

		//load the vertex data
		glEnableVertexAttribArray(this->positionLoc);
		glVertexAttribPointer(this->positionLoc,
			3, // write 3 values
			GL_FLOAT, // they are floats
			GL_FALSE, // not normals
			11 * sizeof(GLfloat), //now many bytes till the next
			0 // where does it start
		); // use 3 values, but add 5 each time to get to the next

		// Load the texture coordinate
		glEnableVertexAttribArray(this->texCoordLoc);
		glVertexAttribPointer(this->texCoordLoc,
			2,
			GL_FLOAT,
			GL_FALSE,
			11 * sizeof(GLfloat),
			(GLfloat*)(9 * sizeof(GLfloat))
		);

		// todo why should we enable attrib array
		if (this->normalLoc != -1)
		{
			//load the normal data
			glEnableVertexAttribArray(this->normalLoc);
			glVertexAttribPointer(this->normalLoc,
				3,
				GL_FLOAT, // they are floats
				GL_TRUE, // not normals
				11 * sizeof(GLfloat),
				(GLfloat*)(3 * sizeof(GLfloat))
			);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		if ((o.material_id < m_mesh_model->m_materials.size())) {
			std::string diffuse_texname = m_mesh_model->m_materials[o.material_id].diffuse_texname;
			if (m_mesh_model->m_textures.find(diffuse_texname) != m_mesh_model->m_textures.end()) {
				glBindTexture(GL_TEXTURE_2D, m_mesh_model->m_textures[diffuse_texname]);
			}
		}

		glUniform1i(this->samplerLoc, 0);

		glDrawArrays(GL_TRIANGLES, 0, o.numTriangles * 3);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// todo fix shading
	m_shader->set_int("light_debug", 0);

	return true;
}

void PlayerSpace::HandleShipInput(const float dt)
{
	float dt_forward_accel = m_forward_accel * dt;
	glm::vec3 dt_euler_accel = m_euler_accel * dt;
	glm::vec3 dt_euler_deaccel = m_euler_deaccel * dt;

	if (m_up_key)
	{
		m_forward_force += dt_forward_accel;
	}
	if (m_down_key)
	{
		m_forward_force += -dt_forward_accel;
	}

	if (m_left_key)
	{
		m_euler_force.z += -dt_euler_accel.z;
	}
	if (m_right_key)
	{
		m_euler_force.z += dt_euler_accel.z;
	}

	// reset roll when no left/right has been pressed
	if (!m_right_key && !m_left_key)
		if (m_euler_force.z < -FLT_EPSILON)
			m_euler_force.z += dt_euler_deaccel.z;
		else if (m_euler_force.z > FLT_EPSILON)
			m_euler_force.z += -dt_euler_deaccel.z; // 0

	// add mouse movement
	m_euler_force.y += -(dt_euler_accel.y * m_hor_mouse_vel);
	m_euler_force.x += -(dt_euler_accel.x * m_ver_mouse_vel);

	// limit forces of forward and rotation
	if (m_forward_force > m_forward_force_limit)
		m_forward_force = m_forward_force_limit;
	if (m_forward_force < -m_backwards_force_limit)
		m_forward_force = -m_backwards_force_limit;
	for (unsigned int i = 0; i < 3; i++)
	{
		if (m_euler_force[i] > m_euler_force_max_limit[i])
			m_euler_force[i] = m_euler_force_max_limit[i];
		else if (m_euler_force[i] < -m_euler_force_max_limit[i])
			m_euler_force[i] = -m_euler_force_max_limit[i];
	}

	// todo add auto stabilize ship logic?
	if (m_stabilize_ship_key)
	{
		m_euler_force = glm::vec3(0.0f);
	}

	GetTransform().Translate(GetTransform().GetForward() * m_forward_force);
	GetTransform().SetEulerAngles(m_euler_force);
}

/*
spaceship Goals
- Alter FOV depending on movement force
*/
void PlayerSpace::UpdateShipCamera(const float dt)
{
	cam_last_world_pos += (GetTransform().GetPosition() - cam_last_world_pos) * cam_pos_lerp_time;

	cam_last_world_look_at_pos += (GetTransform().GetPosition() - cam_last_world_look_at_pos) + cam_rot_lerp_time;

	cam_last_effect_pos += (glm::vec3((m_euler_force.y + (m_euler_force.z * roll_cam_sway_multiplier)) * 200,
		m_euler_force.x * -200, 0) - cam_last_effect_pos) * cam_effect_lerp_time;

	cam_last_up_dir += (GetTransform().GetUp() - cam_last_up_dir) * cam_up_dir_lerp_time;

	glm::vec3 rotated_cam_offset = glm::vec3((GetTransform().GetRotationMatrix() *
		glm::vec4(camera_offset + cam_last_effect_pos, 0)));
	glm::vec3 rotated_look_at_offset = glm::vec3((GetTransform().GetRotationMatrix() *
		glm::vec4(camera_look_at_offset + cam_last_effect_pos, 0)));

	m_active_camera->set_world_pos(cam_last_world_pos + rotated_cam_offset);
	m_active_camera->set_world_look_at_pos(cam_last_world_look_at_pos + rotated_look_at_offset);
	m_active_camera->set_up_dir(cam_last_up_dir);
}

void PlayerSpace::UpdateUniforms(const Camera& the_active_camera)
{
	m_shader->use();

	m_shader->set_vec3("light_pos", glm::vec3(0.0f, 0.0f, 0.0f));
	m_shader->set_int("light_debug", 1);
	
	// make the MVP
	m_shader->set_vec4("ambient", { 1.0f, 1.0f, 0.5f, 1.0f });
	
	const glm::mat4 mvp = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4() * GetTransform().GetModelMatrix(); // Remember order seems backwards
	m_shader->set_mat4("MVP", mvp);

	m_shader->set_mat4("M", GetTransform().GetModelMatrix());

	const glm::mat4 normal_mat = glm::transpose(glm::inverse(GetTransform().GetModelMatrix()));
	m_shader->set_mat4("normal_matrix", normal_mat);
}

void PlayerSpace::UpdateDock(float dt)
{
	if (!m_first_call)
	{
		m_first_call = true;
		last_dock_pos = GetTransform().GetPosition();
		last_orient = GetTransform().GetOrientation();

		GetTransform().m_space_movement = true;
	}
	// todo only do this once per few frames
	m_dock_pos = m_pSpace_station->GetLocalDockPosition();

	glm::quat up_orient = glm::toQuat(glm::orientate3(m_pSpace_station->GetTransform().GetUp()));
	glm::quat forward_orient = glm::toQuat(glm::orientate3(m_pSpace_station->GetTransform().GetForward()));

	last_dock_pos += (m_dock_pos - last_dock_pos) * 0.03f;

	glm::quat new_orient = glm::slerp(last_orient, up_orient, 0.1f);

	GetTransform().SetPosition(last_dock_pos);
	last_orient = GetTransform().GetOrientation();

	UpdateShipCamera(dt);
}

float PlayerSpace::DistFromDock()
{
	return glm::distance(GetTransform().GetPosition(), m_dock_pos); // heavy sqrt
}
