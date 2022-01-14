#pragma once

//Wrapper for graphic loading, do not use for binary or text files.

#include <iostream> // we're going to be loading files so need these
// https://stackoverflow.com/questions/5781597/incomplete-type-is-not-allowed-stringstream
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

class MyFiles
{
public:
	MyFiles();
	~MyFiles();

	int height;
	int width;
	int comp;

	char* Load(char const* filename, int*, int*); // specifically for graphics

	const std::string LoadText(char const* filename) const;
};
