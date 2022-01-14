#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "MD2_defines.h"

// This is our face structure.  This is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
	int vertIndex[3];			// indices for the verts that make up this triangle
	int coordIndex[3];			// indices for the tex coords to texture this face
	int normalIndex;
};

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3
{
public:
	float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2
{
public:
	float x, y;
};


// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object
	CVector3  *pVerts;			// The object's vertices
	CVector3  *pNormals;		// The object's normals
	CVector2  *pTexVerts;		// The texture's UV coordinates
	tFace *pFaces;				// The face information of the object
	
	glm::vec3 bmin, bmax;		// Min and max vertex to create a bounding box
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	byte  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture  
	float vTile;				// v tiling of texture	
	float uOffset;			    // u offset of texture
	float vOffset;				// v offset of texture
};

// This holds our information for each animation of the Quake model.
// A STL vector list of this structure is created in our t3DModel structure below.
struct tAnimationInfo
{
	// strname headers
	char strName[125];			// This stores the name of the animation (Jump, Pain, etc..)
	int startFrame;				// This stores the first frame number for this animation
	int endFrame;				// This stores the last frame number for this animation
};

//////////////////////////////////////////// *** MD2 STRUCTS *** ////////////////////////////////////////////

// This holds the header information that is read in at the beginning of the file
struct tMd2Header
{
	int magic;					// This is used to identify the file
	int version;					// The version number of the file (Must be 8)
	int skinWidth;				// The skin width in pixels
	int skinHeight;				// The skin height in pixels
	int frameSize;				// The size in bytes the frames are
	int numSkins;				// The number of skins associated with the model
	int numVertices;				// The number of vertices (constant for each frame)
	int numTexCoords;			// The number of texture coordinates
	int numTriangles;			// The number of faces (polygons)
	int numGlCommands;			// The number of GL commands
	int numFrames;				// The number of animation frames
	int offsetSkins;				// The offset in the file for the skin data
	int offsetTexCoords;			// The offset in the file for the texture data
	int offsetTriangles;			// The offset in the file for the face data
	int offsetFrames;			// The offset in the file for the frames data
	int offsetGlCommands;		// The offset in the file for the GL commands data
	int offsetEnd;				// The end of the file offset
};

// This is used to store the vertices that are read in for the current frame
struct tMd2AliasTriangle
{
	byte vertex[3];
	byte lightNormalIndex;
};

// This stores the normals and vertices for the frames
struct tMd2Triangle
{
	float vertex[3];
	float normal[3];
};

// This stores the indices into the vertex and texture coordinate arrays
struct tMd2Face
{
	short vertexIndices[3];
	short textureIndices[3];
};

// This stores UV coordinates
struct tMd2TexCoord
{
	short u, v;
};

// This stores the animation scale, translation and name information for a frame, plus verts
struct tMd2AliasFrame
{
	float scale[3];
	float translate[3];
	char name[16];
	tMd2AliasTriangle aliasVertices[1];
};

// This stores the frames vertices after they have been transformed
struct tMd2Frame
{
	char strName[16];
	tMd2Triangle *pVertices;
};
