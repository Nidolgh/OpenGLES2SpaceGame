#pragma once
#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif // RASPBERRY

#include <Utils/TinyObj/tiny_obj_loader.h>
#include "Utils/TriangleCollection/Model.h"

#include <vector>
#include <map>

// typedef struct material_t;

class MeshModel : public Model
{
public:
	typedef struct {
		GLuint vb_id;  // vertex buffer id
		int numTriangles;
		size_t material_id;
	} DrawObject;

	std::vector<DrawObject> m_drawObjects;
	std::vector<tinyobj::material_t> m_materials; // tinyobj::material_t
	std::map<std::string, GLuint> m_textures;
	
	MeshModel();
	virtual ~MeshModel();
};