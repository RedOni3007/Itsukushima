/*
* Load and maintain all the resouces, yes all the resouces
* todo: seperate meshes which data won't change and meshes which data will change (like cutting)
*
* @author: Kai Yang
*/

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <Core/CoreHeaders.h>
#include <Resource/ResourceStructDefines.h>

#include <Core/GLEW_GLFW.h>

#include <glm/glm.hpp>
#include <vector>


class Mesh;
class CollisionMesh;

class ResourceManager
{
private:
	ResourceManager(void);
	~ResourceManager(void);

public:
	static ResourceManager* Instance();

	void Init();
	void GraphicUpdate();

	GLuint LoadShader(const char * pszVertexFN, const char * pszFragmentFN, const char * pszGeometryFN);

	bool LoadMesh(const char * pszMeshPath, const char* pszMeshName, bool bInvertUV = false);
	bool LoadCollisionMesh(const char * pszMeshPath, const char* pszMeshName);

	RefCountPtr<Mesh> CreateMesh(const char* pszMeshName);
	RefCountPtr<CollisionMesh> CreateCollisionMesh(const char* pszMeshName);

	Material*	GetMaterial(uint32 uIndex);
	Material*	GetMaterial(const char* pszName);
	int32		GetMaterialIndex(const char* pszName);

	Texture* GetTexture(uint32 uIndex);
	Texture* GetTexture(const char* pszName);
	int32	 GetTextureIndex(const char* pszName);

	bool CreateTexture(const char* pszResourceName, uint32 uWidth, uint32 uHeight);
	bool LoadTexture(const char* pszFN, const char* pszResourceName);
	Material* CreateMaterial(const char* pszName);//create then set the values

	MetaMesh* GetMetaMesh(const char* pszName);
	MetaCollisionMesh* GetMetaCollisionMesh(const char* pszName);

private:
	void UpdateTexture(Texture* pTexture);

private:
	std::vector<MetaMesh*>			m_metaMeshList;
	std::vector<MetaCollisionMesh*>	m_metaCollisionMeshList;
	std::vector<Material*>			m_materialList;
	std::vector<Texture*>			m_textureList;
};

#endif