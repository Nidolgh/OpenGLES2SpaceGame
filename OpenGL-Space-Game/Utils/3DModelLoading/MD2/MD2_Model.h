#pragma once
#include "MD2_defines.h"
#include "MD2_structs.h"
#include <vector>
#include "Utils/GL_include.h"
#include <Utils/TriangleCollection/Model.h>

class MD2_Model : 
	public Model
{
public:
	MD2_Model() { }
	~MD2_Model() {}
	
	int numOfObjects;					// The number of objects in the model therefore number of frames
	int numOfMaterials;					// The number of materials for the model
	int numOfAnimations;				// The number of animations in this model (NEW)
	
	//int numOfFrames;

	std::vector<tAnimationInfo> pAnimations; // The list of animations (NEW)
	std::vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	std::vector<t3DObject> pObject;			// The object list for our model
	
	GLuint vbo;							//Massive vbo with sub-vbos for each keyframe

	GLsizei vertexDataStride; // the exact stride size (GLFloat)
	GLsizei texCoordDataStride;
	GLsizei normalDataStride;
};