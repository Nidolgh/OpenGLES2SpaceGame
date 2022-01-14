#include "Headers/Planet.h"

Planet::Planet(Model* the_mesh, Shader* the_shader, PlanetType planet_type)
	: DrawableObject(the_shader)
{
	m_planet_type = planet_type;

	m_mesh = dynamic_cast<MeshModel*>(the_mesh);
	Init();
	
	GetTransform().m_use_offset_pos = true;
}

Planet::~Planet()
{
	
}

bool Planet::Init()
{
	m_velocity = glm::vec3(0.0f, 0.0f, 1.0f) * MOS();

	return true;
}

bool Planet::Update(float dt)
{
	tt += dt;

	if (!m_is_alive || m_planet_type == Planet::PlanetGassCloud)
		return true;


	GetTransform().Rotate(glm::vec3(0.0f, 0.0f, (dt * 0.5f)));

	switch (m_planet_type)
	{
	case Planet::PlanetChar:
	{
		GetTransform().Rotate(glm::vec3(0.0f, (dt * 0.1f), 0.0f));
		GetTransform().SetScale(
			glm::vec3(
				(sinf(tt) + 2.0f) * 20.0f, 
				16.0f, 
				(sinf(tt) + 2.0f) * 20.0f
			)
		);
	}
	case Planet::PlanetAsteroid:
	case Planet::PlanetNormal:
	{
		// great help setting up with Jan-Mathijs
		glm::vec3 dir_to_sun = glm::normalize(glm::vec3(0.0f) - GetTransform().GetPosition());
		m_velocity += dir_to_sun * (NLOG() / m_mass);
		GetTransform().SetPosition(GetTransform().GetPosition() + m_velocity);
		break; 
	}
	case Planet::PlanetSun:
	case Planet::PlanetStation:
		break;
	default:
		break;
	}

	return true;
}

// utilizing the explained method for drawing with tiny-obj format: https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
bool Planet::Draw(const Camera& the_active_camera)
{
	if (!m_is_alive)
		return true;

	if (m_mesh == nullptr)
		return false;

	// only update
	UpdateUniforms(the_active_camera);

	for (size_t i = 0; i < m_mesh->m_drawObjects.size(); i++)
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
		if (this->texCoordLoc != -1)
		{
			glEnableVertexAttribArray(this->texCoordLoc);
			glVertexAttribPointer(this->texCoordLoc,
				2,
				GL_FLOAT,
				GL_FALSE,
				11 * sizeof(GLfloat),
				(GLfloat*)(9 * sizeof(GLfloat))
			);
		}

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

		int error = 0;
		if ((error = glGetError()) != GL_NO_ERROR)
		{
			printf("Planet calls: %i\n", error);
		}
	}

	return true;
}

// can utilize decorator design pattern
void Planet::UpdateUniforms(const Camera& the_active_camera)
{
	m_shader->use();

	m_shader->set_vec4("ambient", m_ambient_col);
	m_shader->set_vec4("main_col", m_main_col);
	m_shader->set_bool("is_shaded", m_is_shaded);

	const glm::mat4 mvp = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4() * GetTransform().GetModelMatrix(); // Remember order seems backwards
	m_shader->set_mat4("MVP", mvp);

	m_shader->set_mat4("M", GetTransform().GetModelMatrix());

	const glm::mat4 normal_mat = glm::transpose(glm::inverse(GetTransform().GetModelMatrix()));
	m_shader->set_mat4("normal_matrix", normal_mat);

	if (m_planet_type == PlanetType::PlanetGassCloud)
		m_shader->set_float("time", tt);
}

float Planet::MOS()
{
	// double for precision
	double distance = glm::distance(GetTransform().GetPosition(), glm::vec3(0.0f));
	float vel_speed = sqrt((m_GRAVITY * 5000) / distance);
	return vel_speed;
}

float Planet::NLOG() // Newton Law Of Gravity
{
	float distance = glm::distance(GetTransform().GetPosition(), glm::vec3(0.0f));
	float m_force = m_GRAVITY * ((m_mass * 5000) / (distance * distance));
	return m_force;
}
