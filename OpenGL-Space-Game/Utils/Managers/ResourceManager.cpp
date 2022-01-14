#include "ResourceManager.h"
#include "Utils/3DModelLoading/MD2/MD2_Model.h"


ResourceManager::ResourceManager() :
	m_RESOURCEPATH(GetResourcePath()), 
	m_md2_loader(&m_texture_map)
{

}

ResourceManager::~ResourceManager()
{
	// clean the memory
	{
		std::map< std::string, MeshModel* >::iterator it;
		for (it = m_obj_map.begin(); it != m_obj_map.end(); it++)
		{
			printf("deleting %s\n", it->first);
			delete it->second;
		}
	}

	{
		std::map< std::string, MD2_Model* >::iterator it;
		for (it = m_md_map.begin(); it != m_md_map.end(); it++)
		{
			printf("deleting %s\n", it->first);
			delete it->second;
		}
	}

	{
		std::map< std::string, Shader* >::iterator it;
		for (it = m_shader_map.begin(); it != m_shader_map.end(); it++)
		{
			delete it->second;
		}
	}
}

void ResourceManager::Init(BaseWindow* pBase_window)
{
	m_pBase_window = pBase_window;
}

// https://github.com/syoyo/tinyobjloader/blob/master/examples/viewer/viewer.cc
// https://github.com/syoyo/tinyobjloader
bool ResourceManager::LoadObjAndConvert(std::vector<MeshModel::DrawObject>* draw_objects,
	std::vector<tinyobj::material_t>& materials,
	std::map<std::string, GLuint>& textures,
	const char* filename, glm::vec3& bmin, glm::vec3& bmax)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;

	// get complete base path
	std::string base_dir = GetBaseDir(filename);
	if (base_dir.empty()) {
		base_dir = ".";
	}
	// add a separator
	base_dir += "/";

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,
		base_dir.c_str());
	if (!warn.empty())
		std::cout << warn << std::endl;

	if (!err.empty())
		std::cerr << err << std::endl;

	if (!ret)
	{
		std::cerr << "Failed to load " << filename << std::endl;
		return false;
	}

	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());
	printf("Basedir = %s\n", base_dir);

	// Append `default` material
	materials.push_back(tinyobj::material_t());

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%d].diffuse_texname = %s\n", int(i),
			materials[i].diffuse_texname.c_str());
	}

	// Load diffuse textures
	{
		for (size_t m = 0; m < materials.size(); m++) {
			tinyobj::material_t* mp = &materials[m];

			if (mp->diffuse_texname.length() > 0) {
				// Only load the texture if it is not already loaded
				if (textures.find(mp->diffuse_texname) == textures.end()) {
					GLuint texture_id;
					int w, h;
					int comp;

					std::string texture_filename = mp->diffuse_texname;
					if (!FileExists(texture_filename)) {
						// Append base dir.
						texture_filename = base_dir + mp->diffuse_texname;
						if (!FileExists(texture_filename)) {
							std::cerr << "Unable to find file: " << mp->diffuse_texname
								<< std::endl;
							exit(1);
						}
					}

					unsigned char* image =
						stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
					if (!image) {
						std::cerr << "Unable to load texture: " << texture_filename
							<< std::endl;
						exit(1);
					}
					std::cout << "Loaded texture: " << texture_filename << ", w = " << w
						<< ", h = " << h << ", comp = " << comp << std::endl;

					glGenTextures(1, &texture_id);
					glBindTexture(GL_TEXTURE_2D, texture_id);
					//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					// mipmaps from Brian race game demo 
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

					if (comp == 3) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
							GL_UNSIGNED_BYTE, image);
					}
					else if (comp == 4) {
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
							GL_UNSIGNED_BYTE, image);
					}
					else {
						assert(0);  // TODO
					}
					// mipmaps
					glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);
					stbi_image_free(image);
					textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
				}
			}
		}
	}

	bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
	bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

	m_temp_vertex_buffer.clear();
	for (size_t s = 0; s < shapes.size(); s++) {
		MeshModel::DrawObject o;
		std::vector<float> temp_buffer;  // pos(3float), normal(3float), color(3float)

		// Check for smoothing group and compute smoothing normals
		/*std::map<int, vec3> smoothVertexNormals;
		if (hasSmoothingGroup(shapes[s]) > 0) {
			std::cout << "Compute smoothingNormal for shape [" << s << "]" << std::endl;
			computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
		}*/

		for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
			tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
			tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
			tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

			int current_material_id = shapes[s].mesh.material_ids[f];

			if ((current_material_id < 0) ||
				(current_material_id >= static_cast<int>(materials.size()))) {
				// Invaid material ID. Use default material.
				current_material_id =
					materials.size() -
					1;  // Default material is added to the last item in `materials`.
			}

			float diffuse[3];
			for (size_t i = 0; i < 3; i++) {
				diffuse[i] = materials[current_material_id].diffuse[i];
			}
			float tc[3][2];
			if (attrib.texcoords.size() > 0) {
				if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
					(idx2.texcoord_index < 0)) {
					// face does not contain valid uv index.
					tc[0][0] = 0.0f;
					tc[0][1] = 0.0f;
					tc[1][0] = 0.0f;
					tc[1][1] = 0.0f;
					tc[2][0] = 0.0f;
					tc[2][1] = 0.0f;
				}
				else {
					assert(attrib.texcoords.size() >
						size_t(2 * idx0.texcoord_index + 1));
					assert(attrib.texcoords.size() >
						size_t(2 * idx1.texcoord_index + 1));
					assert(attrib.texcoords.size() >
						size_t(2 * idx2.texcoord_index + 1));

					// Flip Y coord.
					tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
					tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
					tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
					tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
					tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
					tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
				}
			}
			else {
				tc[0][0] = 0.0f;
				tc[0][1] = 0.0f;
				tc[1][0] = 0.0f;
				tc[1][1] = 0.0f;
				tc[2][0] = 0.0f;
				tc[2][1] = 0.0f;
			}

			float v[3][3];
			for (int k = 0; k < 3; k++) {
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);

				v[0][k] = attrib.vertices[3 * f0 + k];
				v[1][k] = attrib.vertices[3 * f1 + k];
				v[2][k] = attrib.vertices[3 * f2 + k];
				bmin[k] = glm::min(v[0][k], bmin[k]);
				bmin[k] = glm::min(v[1][k], bmin[k]);
				bmin[k] = glm::min(v[2][k], bmin[k]);
				bmax[k] = glm::max(v[0][k], bmax[k]);
				bmax[k] = glm::max(v[1][k], bmax[k]);
				bmax[k] = glm::max(v[2][k], bmax[k]);
			}

			float n[3][3];
			{
				bool invalid_normal_index = false;
				if (attrib.normals.size() > 0) {
					int nf0 = idx0.normal_index;
					int nf1 = idx1.normal_index;
					int nf2 = idx2.normal_index;

					if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
						// normal index is missing from this face.
						invalid_normal_index = true;
					}
					else {
						for (int k = 0; k < 3; k++) {
							assert(size_t(3 * nf0 + k) < attrib.normals.size());
							assert(size_t(3 * nf1 + k) < attrib.normals.size());
							assert(size_t(3 * nf2 + k) < attrib.normals.size());
							n[0][k] = attrib.normals[3 * nf0 + k];
							n[1][k] = attrib.normals[3 * nf1 + k];
							n[2][k] = attrib.normals[3 * nf2 + k];
						}
					}
				}
				else {
					invalid_normal_index = true;
				}

				//if (invalid_normal_index && !smoothVertexNormals.empty()) {
				//    // Use smoothing normals
				//    int f0 = idx0.vertex_index;
				//    int f1 = idx1.vertex_index;
				//    int f2 = idx2.vertex_index;

				//    if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
				//        n[0][0] = smoothVertexNormals[f0].v[0];
				//        n[0][1] = smoothVertexNormals[f0].v[1];
				//        n[0][2] = smoothVertexNormals[f0].v[2];

				//        n[1][0] = smoothVertexNormals[f1].v[0];
				//        n[1][1] = smoothVertexNormals[f1].v[1];
				//        n[1][2] = smoothVertexNormals[f1].v[2];

				//        n[2][0] = smoothVertexNormals[f2].v[0];
				//        n[2][1] = smoothVertexNormals[f2].v[1];
				//        n[2][2] = smoothVertexNormals[f2].v[2];

				//        invalid_normal_index = false;
				//    }
				//}

				//if (invalid_normal_index) {
				//    // compute geometric normal
				//    CalcNormal(n[0], v[0], v[1], v[2]);
				//    n[1][0] = n[0][0];
				//    n[1][1] = n[0][1];
				//    n[1][2] = n[0][2];
				//    n[2][0] = n[0][0];
				//    n[2][1] = n[0][1];
				//    n[2][2] = n[0][2];
				//}
			}

			for (int k = 0; k < 3; k++) {
				temp_buffer.push_back(v[k][0]);
				temp_buffer.push_back(v[k][1]);
				temp_buffer.push_back(v[k][2]);
				temp_buffer.push_back(n[k][0]);
				temp_buffer.push_back(n[k][1]);
				temp_buffer.push_back(n[k][2]);
				// Combine normal and diffuse to get color.
				float normal_factor = 0.2;
				float diffuse_factor = 1 - normal_factor;
				float c[3] = { n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
							  n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
							  n[k][2] * normal_factor + diffuse[2] * diffuse_factor };
				float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
				if (len2 > 0.0f) {
					float len = sqrtf(len2);

					c[0] /= len;
					c[1] /= len;
					c[2] /= len;
				}
				temp_buffer.push_back(c[0] * 0.5 + 0.5);
				temp_buffer.push_back(c[1] * 0.5 + 0.5);
				temp_buffer.push_back(c[2] * 0.5 + 0.5);

				temp_buffer.push_back(tc[k][0]);
				temp_buffer.push_back(tc[k][1]);
			}
		}

		o.vb_id = 0;
		o.numTriangles = 0;

		// OpenGL viewer does not support texturing with per-face material.
		if (shapes[s].mesh.material_ids.size() > 0 &&
			shapes[s].mesh.material_ids.size() > s) {
			o.material_id = shapes[s].mesh.material_ids[0];  // use the material ID
															 // of the first face.
		}
		else {
			o.material_id = materials.size() - 1;  // = ID for default material.
		}
		printf("shape[%d] material_id %d\n", int(s), int(o.material_id));

		if (temp_buffer.size() > 0) {
			glGenBuffers(1, &o.vb_id);
			glBindBuffer(GL_ARRAY_BUFFER, o.vb_id);
			glBufferData(GL_ARRAY_BUFFER, temp_buffer.size() * sizeof(float),
				&temp_buffer.at(0), GL_STATIC_DRAW);
			o.numTriangles = temp_buffer.size() / (3 + 3 + 3 + 2) /
				3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

			printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
				o.numTriangles);
		}

		draw_objects->push_back(o);
		m_temp_vertex_buffer.push_back(temp_buffer);
	}

	printf("bmin = %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
	printf("bmax = %f, %f, %f\n", bmax[0], bmax[1], bmax[2]);

	return true;
}

MeshModel* ResourceManager::LoadObj(const std::string file_path)
{
	glm::vec3 min, max; min = max = glm::vec3(0.0f);
	MeshModel* foo = new MeshModel();
	LoadObjAndConvert(&foo->m_drawObjects, 
		foo->m_materials, 
		foo->m_textures, 
		file_path.c_str(), min, max);
	foo->Init(min, max);
	return foo;
}

MD2_Model* ResourceManager::LoadMD2(const std::string file_path)
{
	std::string temp_tex_path = ResourceFileHandling("Models/modelpack0/samourai/Samourai.jpg");

	MD2_Model* the_model = new MD2_Model();
	bool test = m_md2_loader.ImportMD2(the_model, file_path.c_str(), temp_tex_path.c_str(), "samurai-tex");
	if (test)
		printf("MD2 loading succesfull\n");
	else
		printf("MD2 loading failed\n");
	return the_model;
}

AnimatedModel* ResourceManager::LoadMD5(const std::string file_path)
{
	return nullptr;
}

bool ResourceManager::FileExists(const std::string& abs_filename)
{
	bool ret;

#ifdef RASPBERRY
	FILE* fp = fopen(abs_filename.c_str(), "rb");
#else
	FILE* fp;
	fopen_s(&fp, abs_filename.c_str(), "rb");
#endif

	if (fp) {
		ret = true;
		fclose(fp);
	}
	else {
		ret = false;
	}

	return ret;
}

std::string ResourceManager::ResourceFileHandling(const std::string resource_path)
{
	return m_RESOURCEPATH + resource_path;
}

std::string ResourceManager::GetResourcePath()
{
#ifdef RASPBERRY
	return "/home/pi/projects/Space_Game_Linux/Resources/";
#else
	if (!m_RESOURCEPATH.empty())
		return m_RESOURCEPATH;

	std::string base = "";

	// // get a correct resource folder on windows utilizing GetModuleFileName
	// get path to exe
	// https://stackoverflow.com/questions/18783087/how-to-properly-use-getmodulefilename
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	// convert wchar_t to std::string using std::wstring
	// https://stackoverflow.com/questions/27720553/conversion-of-wchar-t-to-string
	std::wstring ws(buffer);
	std::string str(ws.begin(), ws.end());

	// replace all \\ to /
	// https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
	bool done = false;
	while (!done)
	{
		std::string toReplace("\\");
		int pos = str.find(toReplace);
		if (pos == -1) done = true; else
			str.replace(pos, toReplace.length(), "/");
	}

	// get rid of final 3 directories and add our resource file to it, feels like overdoing it
	done = false;
	unsigned int endpos = str.size();
	unsigned int offsetpos = 0;
	int dirpos = 0;
	int dirskipamount = 3;
	while (!done)
	{
		if (str[endpos - offsetpos] == '/')
		{
			dirpos++;
			if (dirpos >= dirskipamount)
				done = true;
		}
		offsetpos++;
	}

	// http://www.cplusplus.com/reference/string/string/erase/
	str.erase(endpos - (offsetpos - 2), std::string::npos); // - 2 as some dir chars were lost
	str += "Resources/";

	base = str;

	// harddir
	return base;

#endif // RASPBERRY
}

Model* ResourceManager::LoadModel(std::string file_path)
{
	// http://www.cplusplus.com/reference/map/map/find/
	std::map< std::string, MeshModel* >::iterator it;
	it = m_obj_map.find(file_path);
	if (it != m_obj_map.end())
	{
		return (*it).second;
	}
	else
	{
		if (file_path.find(".obj") != std::string::npos)
		{
			// ways to add to a map https://stackoverflow.com/questions/4286670/what-is-the-preferred-idiomatic-way-to-insert-into-a-map
			MeshModel* bar = LoadObj(ResourceFileHandling(file_path));
			m_obj_map.insert({ file_path, bar });
			return bar;
		}
		else if (file_path.find(".md2") != std::string::npos)
		{
			MD2_Model* bar = LoadMD2(ResourceFileHandling(file_path));
			m_md_map.insert({ file_path, bar });
			return bar;
		}
		else if (file_path.find(".md5") != std::string::npos)
		{
			/*AnimatedModel* bar = LoadMD5(ResourceFileHandling(file_path));
			m_md_map.insert({ file_path, bar });
			return bar;*/
		}
	}
}

Shader* ResourceManager::LoadShader(const std::string vert_path)
{
	std::string frag_path = vert_path;
	std::string toReplace(".vert");
	int pos = frag_path.find(toReplace);
	if (pos != -1)
		frag_path.replace(pos, toReplace.length(), ".frag");

	return LoadShader(vert_path, frag_path);
}

Shader* ResourceManager::LoadShader(const std::string vert_path, const std::string frag_path)
{
	std::map< std::string, Shader* >::iterator it;
	it = m_shader_map.find(vert_path);
	if (it != m_shader_map.end())
	{
		return it->second;
	}
	else
	{
		Shader* bar = new Shader(ResourceFileHandling(vert_path).c_str(), ResourceFileHandling(frag_path).c_str());

		m_shader_map.insert({ vert_path, bar });
		return bar;
	}

}

bool ResourceManager::LoadAndSetCubeMapTexture(const char file_path[255], GLuint texture_loc, GLenum cube_map_target)
{
	std::string file_complete_path = ResourceFileHandling(file_path);
	if (FileExists(file_complete_path)) {
		int w, h, comp;
		unsigned char* image = stbi_load(file_complete_path.c_str(), &w, &h, &comp, STBI_default);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (comp == 3) {
			glTexImage2D(cube_map_target, 0, GL_RGB, w, h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);
		}
		else if (comp == 4) {
			glTexImage2D(cube_map_target, 0, GL_RGBA, w, h, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, image);
		}
		else {
			assert(0);  // TODO
		}

		// glTexImage2D(cube_map_target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(image);

		return true;
	}
	return false;
}

unsigned int ResourceManager::GetWindowWidth()
{
	return m_pBase_window->GetWidth();
}

unsigned int ResourceManager::GetWindowHeight()
{
	return m_pBase_window->GetHeight();
}
