#pragma once
#include "Utils/TriangleCollection/MeshModel.h"
#include "Utils/TriangleCollection/AnimatedModel.h"

#ifdef RASPBERRY
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>

// apperantly including win32 related also includes their own untemplated min max methods that will interfere with glm https://en.sfml-dev.org/forums/index.php?topic=10216.0
#define NOMINMAX
#include <Windows.h>
#endif // RASPBERRY

#include <iostream>

#include "glm/glm.hpp"

#include "Utils/TinyObj/tiny_obj_loader.h"
#include "Utils/ImageLoading/stb_image.h"

#include "Utils/Shader/Shader.h"

#include "Utils/Window/BaseWindow.h"

#include <Utils/3DModelLoading/MD2/MD2_Loader.h>

class ResourceManager
{
protected:
	const std::string m_RESOURCEPATH;

	std::map< std::string, MeshModel* > m_obj_map;
	std::map< std::string, MD2_Model* > m_md_map;
	std::map< std::string, Shader* > m_shader_map;

	std::map<std::string, Texture*> m_texture_map;

	// obj loading, based on TinyObj examples
	// todo: have a dedicated .obj loader which could also decide on loading md5, etc files
	bool			LoadObjAndConvert(std::vector<MeshModel::DrawObject>* draw_objects,
						std::vector<tinyobj::material_t>& materials,
						std::map<std::string, GLuint>& textures,
						const char* filename, glm::vec3& bmin, glm::vec3& bmax);

	MeshModel*		LoadObj(const std::string file_path);
	MD2_Model*		LoadMD2(const std::string file_path);
	AnimatedModel*	LoadMD5(const std::string file_path);

	std::string GetResourcePath();
	
	BaseWindow* m_pBase_window;

	MD2_Loader m_md2_loader;

	std::string GetBaseDir(const std::string& filepath) {
		if (filepath.find_last_of("/\\") != std::string::npos)
			return filepath.substr(0, filepath.find_last_of("/\\"));
		return "";
	}
	bool FileExists(const std::string& abs_filename);

	/*typedef struct
	{
		int width;
		int height;
		int comp;
	} ImageData;*/

public:
	ResourceManager();
	~ResourceManager();

	void Init(BaseWindow* pBase_window);

	std::string ResourceFileHandling(const std::string file_path);

	Model* LoadModel(const std::string file_path);

	// a method that only needs vert auto creates a frag path on the same location
	Shader* LoadShader(const std::string vert_path);
	Shader* LoadShader(const std::string vert_path, const std::string frag_path);

	bool LoadAndSetCubeMapTexture(const char file_path[255], GLuint texture_loc, GLenum cube_map_target);

	unsigned int GetWindowWidth();
	unsigned int GetWindowHeight();

	std::vector<std::vector<float>> m_temp_vertex_buffer;  // used for collision setting // pos(3float), normal(3float), color(3float)

};