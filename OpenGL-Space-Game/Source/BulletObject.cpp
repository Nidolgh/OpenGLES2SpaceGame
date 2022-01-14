#include "Headers/BulletObject.h"
BulletObject::BulletObject(Model* the_mesh, Shader* the_shader) :
	Planet(the_mesh, the_shader)
{
	
}

bool BulletObject::Update(float dt)
{
	BaseObject::Update(dt);

	m_phys_obj->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION); // DON'T LET OUR PLAYER GO TO SLEEP, or we can't move it.
	
	m_phys_obj->GetRigidBody()->setDamping(0.2f, 0.15f);

	return true;
}

bool BulletObject::Draw(const Camera& the_active_camera)
{
	Planet::Draw(the_active_camera);
	return true;
}

void BulletObject::UpdateUniforms(const Camera& the_active_camera)
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