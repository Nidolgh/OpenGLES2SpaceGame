#include "Headers/SpaceStation.h"

SpaceStation::SpaceStation(Model* the_mesh, Shader* the_shader) :
	Planet(the_mesh, the_shader, PlanetType::PlanetStation)
{

}

SpaceStation::~SpaceStation()
{

}

bool SpaceStation::Init(const bool is_shaded)
{
	m_is_shaded = is_shaded;
	// base dock offset from scale measured using Blender, y-z must be swapped around
	m_local_dock_position = GetTransform().GetScale() * 
		glm::vec3(0.0f, -0.04f, 0.35f);

	return true;
}

bool SpaceStation::Update(float dt)
{
	Planet::Update(dt);

	GetTransform().SetEulerAngles(glm::vec3(0.0f, -20.0f, 0.0f));

	return true;
}

bool SpaceStation::Draw(const Camera& the_active_camera)
{
	// only update
	UpdateUniforms(the_active_camera);

	int draw_obj_size = m_mesh->m_drawObjects.size();
	if (m_is_dockable) 
		draw_obj_size--; // dont draw last obj which in this special case should be the barrier mesh

	for (size_t i = 0; i < draw_obj_size; i++)
	{
		MeshModel::DrawObject o = m_mesh->m_drawObjects[i];

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
		if (this->normalLoc != -1)
		{
			//load the normal data
			glEnableVertexAttribArray(this->normalLoc);
			glVertexAttribPointer(this->normalLoc,
				3,
				GL_FLOAT, // they are floats
				GL_FALSE, // not normals
				11 * sizeof(GLfloat),
				(GLfloat*)(6 * sizeof(GLfloat))
			);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		if ((o.material_id < m_mesh->m_materials.size())) {
			std::string diffuse_texname = m_mesh->m_materials[o.material_id].diffuse_texname;
			if (m_mesh->m_textures.find(diffuse_texname) != m_mesh->m_textures.end()) {
				glBindTexture(GL_TEXTURE_2D, m_mesh->m_textures[diffuse_texname]);
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

void SpaceStation::UpdateUniforms(const Camera& the_active_camera)
{
	m_shader->use();

	m_shader->set_int("light_debug", 1);

	m_shader->set_vec3("light_pos", glm::vec3(0.0f, 0.0f, 0.0f));

	// make the MVP
	m_shader->set_vec4("ambient", { 1.0f, 1.0f, 0.5f, 1.0f });
	
	const glm::mat4 mvp = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4() * GetTransform().GetModelMatrix(); // Remember order seems backwards
	m_shader->set_mat4("MVP", mvp);

	m_shader->set_mat4("M", GetTransform().GetModelMatrix());

	// todo fix the normal diffuse lighting issue
	const glm::mat4 normal_mat = glm::transpose(glm::inverse(GetTransform().GetModelMatrix()));

	m_shader->set_mat4("normal_matrix", normal_mat);
}

glm::vec3 SpaceStation::GetLocalDockPosition()
{
	return GetTransform().GetPosition() +
		glm::vec3((GetTransform().GetRotationMatrix() * glm::vec4(m_local_dock_position, 0)));
}
