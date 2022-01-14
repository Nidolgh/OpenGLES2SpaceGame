#pragma once
#include "MD2_structs.h"
#include <map>
#include <Utils/GL_include.h>
#include <string>

class MD2_Model;

struct Texture
{
	GLuint m_id;
	int m_height, m_width;
};

class MD2_Loader
{
public:
	MD2_Loader(std::map<std::string, Texture*>* textureMap); // This inits the data members

	// This is the function that you call to load the MD2
	bool ImportMD2(MD2_Model *pModel, const char *strFileName, const char *strTexture, const char *textureName);

protected:
	bool FileExists(const std::string& abs_filename);

private:

    // This reads in the data from the MD2 file and stores it in the member variables
	void ReadMD2Data();

	// This parses the animations name and calculates the number of animations and info
	void ParseAnimations(MD2_Model *pModel);

    // This converts the member variables to our pModel structure
	void ConvertDataStructures(MD2_Model *pModel);

    // Loads in the textures
	void LoadTextures(MD2_Model *pModel);

	// Creates the vbo
	void CreateVBO(MD2_Model *pModel);

    // This frees memory and closes the file
	void CleanUp();

	std::map<std::string, Texture*>* textures;

    // The file pointer
	FILE *m_FilePointer;

    // Member variables		

	tMd2Header				m_Header;			// The header data
	tMd2Skin				*m_pSkins;			// The skin data
	tMd2TexCoord			*m_pTexCoords;		// The texture coordinates
	tMd2Face				*m_pTriangles;		// Face index information
	tMd2Frame				*m_pFrames;			// The frames of animation (vertices)
};