#include "Headers/SkyBox.h"

SkyBox::SkyBox(Shader* the_shader)
	: DrawableObject(the_shader)
{

}

void SkyBox::Init(ResourceManager& pRes_man)
{
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(m_sky_verts),
		m_sky_verts,
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// load textures on a cube map following haroldserrano
	// https://www.haroldserrano.com/blog/how-to-apply-a-skybox-in-opengl
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	for (size_t i = 0; i < 6; i++)
	{
		if (pRes_man.LoadAndSetCubeMapTexture(m_cube_map_textures[i], m_textureID, m_cube_map_target[i]))
		{
			printf("succesfully loaded:%s\n", m_cube_map_textures[i]);
		}
		else
		{
			printf("failed loaded:%s\n", m_cube_map_textures[i]);
		}
	}

	m_uv_map_uniform_loc = m_shader->get_uniform("SkyBoxTexture");
}

bool SkyBox::Update(float dt)
{
	return true;
}

bool SkyBox::Draw(const Camera& the_active_camera)
{
	GetTransform().SetPosition(the_active_camera.get_world_pos_v3());

	m_shader->use();
	UpdateUniforms(the_active_camera);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glVertexAttribPointer(this->positionLoc,
		3, // write 3 values
		GL_FLOAT, // they are floats
		GL_FALSE, // not normals
		5 * sizeof(GLfloat),//now many bytes till the next
		0 // where does it start
	);

	glEnableVertexAttribArray(positionLoc);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	glUniform1i(m_uv_map_uniform_loc, 0);

	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	return true;
}

void SkyBox::UpdateUniforms(const Camera& the_active_camera)
{
	glm::mat4 mvp = the_active_camera.get_projection_m4() * the_active_camera.get_view_m4() * GetTransform().GetModelMatrix(); // Remember order seems backwards
	const GLuint matrix_id = glGetUniformLocation(m_shader->m_id, "MVP");
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
}