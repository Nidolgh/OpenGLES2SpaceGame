#include "Shader.h"

#include <vector>

Shader::Shader()
{
	// glDeleteProgram(m_id);
}

// based partially on: LearnOpenGL https://learnopengl.com/Getting-started/Shaders
// and the template given ShaderManager for loading from file
Shader::Shader(const char vertex_path[255], const char fragment_path[255])
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	// todo ADD error handling for files that failed to open

	// open files
	vShaderFile.open(vertex_path);
	fShaderFile.open(fragment_path);
	std::stringstream vShaderStream, fShaderStream;
	// read file's buffer contents into streams
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	// close file handlers
	vShaderFile.close();
	fShaderFile.close();
	// convert stream into string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();
	
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	
	// print out the shader source code
	//printf("shader loaded from filename %s \n %s\n", vertex_path, vShaderCode);
	//printf("shader loaded from filename %s \n %s\n", fragment_path, fShaderCode);

	int success;
	char infoLog[512];

	// vertex Shader
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	};
	
	// fragment Shader
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	};
	
	// shader Program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex);
	glAttachShader(m_id, fragment);
	glLinkProgram(m_id);
	// print linking errors if any
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		// The maxLength includes the NULL character
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);

		glDeleteProgram(m_id);
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() const
{
	glUseProgram(m_id);
}

void Shader::set_bool(const char name[255], const bool value) const
{
	glUniform1i(glGetUniformLocation(m_id, name), static_cast<int>(value));
}

void Shader::set_int(const char name[255], const int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name), value);
}

void Shader::set_float(const char name[255], const float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name), value);
}

void Shader::set_vec3(const char name[255], const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_vec4(const char name[255], const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_vec4(const char name[255], const float* value) const
{
	glUniform4fv(glGetUniformLocation(m_id, name), 1, &value[0]);
}

void Shader::set_mat3(const char name[255], const glm::mat3& value) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &value[0][0]);
}

void Shader::set_mat4(const char name[255], const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &value[0][0]);
}