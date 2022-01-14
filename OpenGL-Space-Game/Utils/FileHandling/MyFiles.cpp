#include "MyFiles.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/ImageLoading/stb_image.h"

MyFiles::MyFiles()
{

}

MyFiles::~MyFiles()
{

}

char* MyFiles::Load(char const* filename, int* width, int* height)
{
	unsigned char* data = stbi_load(filename, width, height, &comp, 4); // ask it to load 4 componants since its rgba
	return (char*)data;
}

// will load a standard style text file
const std::string MyFiles::LoadText(char const* filename) const
{
	std::ifstream my_file;
	my_file.open(filename);
	
	if (my_file.is_open())
	{
		// read file's buffer contents into streams
		std::stringstream stream;
		stream << my_file.rdbuf();
		my_file.close();
		return stream.str();
	}

	return ""; // return something empty
}