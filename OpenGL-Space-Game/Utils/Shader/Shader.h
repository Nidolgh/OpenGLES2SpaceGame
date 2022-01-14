#pragma once
#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif // RASPBERRY

#include <glm/vec3.hpp>
//#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <string>
#include <sstream>
#include <fstream>

// based on: LearnOpenGL Shader topic https://learnopengl.com/Getting-started/Shaders
class Shader
{
public:
	GLuint m_id;

	Shader();
	Shader(const char vertex_path[255], const char fragment_path[255]);
	
	void use() const;

	GLuint get_uniform(const char check[255]) { return glGetUniformLocation(m_id, check); }
	GLuint get_attrib(const char check[255]) { return glGetAttribLocation(m_id, check); }
	
	void set_bool(const char name[255], const bool value) const;
	void set_int(const char name[255], const int value) const;
	void set_float(const char name[255], const float value) const;
	void set_vec3(const char name[255], const glm::vec3& value) const;
	void set_vec4(const char name[255], const glm::vec4& value) const;
	void set_vec4(const char name[255], const float* value) const;
	void set_mat3(const char name[255], const glm::mat3& value) const;
	void set_mat4(const char name[255], const glm::mat4& value) const;
};
