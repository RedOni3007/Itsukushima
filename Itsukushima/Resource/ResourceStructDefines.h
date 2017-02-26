/*
* The struct defines for resource manager
* todo:some structs should be class
*
* @author: Kai Yang
*/

#ifndef RESOURCE_STRUCT_DEFINES_H
#define RESOURCE_STRUCT_DEFINES_H

#include <Core/CoreHeaders.h>

#include <vector>

struct Face
{
	Vector3 normal;//unnormalized, not sure if need to cache the normal
	uint32 v[3]; // indices of m_vertices
	bool bTouched;//will not reset after test, make sure test all the faces when use this var

	//not need at this moment
	//std::vector<uint32> adjFaces; // indices of m_Faces
	//uint32 index;//index in face array, for quick index finding

	Face(uint32 v0, uint32 v1, uint32 v2)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		bTouched = false;
	};
	Face()
	{
		v[0] = 0;
		v[1] = 1;
		v[2] = 2;
		bTouched = false;
	};
};

struct MetaMesh
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<VertexIndex> indices;

	const char* pszName;
};

struct MetaCollisionMesh
{
	std::vector<glm::vec3> vertices;
	std::vector<VertexIndex> indices;

	float32 fMaxRange;
	float32 fMinX, fMinY, fMinZ;
	float32 fMaxX, fMaxY, fMaxZ;
	float32 fLengthX, fLengthY, fLengthZ;

	const char* pszName;

	MetaCollisionMesh()
	{
		fMaxRange = 0;
		fMinX = fMinY = fMinZ = FLT_MAX;
		fMaxX = fMaxY = fMaxZ = -FLT_MAX;
		fLengthX = fLengthY = fLengthZ = 0;
	}
};

//todo:change material and texture to class
struct Material
{
	glm::vec3	vDiffuseColour;
	glm::vec3	vSpecularColour;
	float32		fShininess;
	float32		bMetallic;

	int32		nDiffuseTextureIndex;
	int32		nSpecularTextureIndex;
	int32		nNormalTextureIndex;
	int32		nProgramIndex;//not in use
	const char* pszName;
};

//todo:tile and animated texture
struct Texture
{
	uint32		uGLID;
	uint32		uWidth;
	uint32		uHeight;

	std::vector<uint8> metaData; 
	const char* pszName;

	bool		bNeedUpdate;

	Texture()
	{
		uGLID  = 0;
		uWidth = 0;
		uHeight = 0;
		pszName = "";
		bNeedUpdate = false;
	}
};


#endif