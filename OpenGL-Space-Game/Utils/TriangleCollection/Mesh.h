#pragma once
#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif // RASPBERRY

#include "glm/glm.hpp"
#include <iostream>

#include "Utils/TinyObj/tiny_obj_loader.h"
#include "Utils/ImageLoading/stb_image.h"

class Mesh
{
public:
	typedef struct {
		GLuint vb_id;  // vertex buffer id
		int numTriangles;
		size_t material_id;
	} DrawObject;

	std::vector<DrawObject> m_drawObjects;
	std::vector<tinyobj::material_t> m_materials;
	std::map<std::string, GLuint> m_textures;
	
	// obj loading, based on TinyObj examples
	// todo: have a dedicated .obj loader which could also decide on loading md5, etc files
	bool FileExists(const std::string& abs_filename);
	std::string GetBaseDir(const std::string& filepath);
	bool LoadObjAndConvert(std::vector<DrawObject>* draw_objects,
		std::vector<tinyobj::material_t>& materials,
		std::map<std::string, GLuint>& textures,
		const char* filename);
	
	Mesh(const char* filename);
	~Mesh();
};