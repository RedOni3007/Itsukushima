#include <Resource/ResourceManager.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <lodepng.h>
#include <map>

#include <Core/StringHelper.h>

#include <Resource/Mesh.h>
#include <Resource/CollisionMesh.h>

struct PackedVertex
{
	Vector3 position;
	Vector2 uv;
	Vector3 normal;

	bool operator<(const PackedVertex that) const
	{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	};
};

bool getSimilarVertexIndex_fast( 
	PackedVertex & packed, 
	std::map<PackedVertex,VertexIndex> & VertexToOutIndex,
	VertexIndex& result)
{
	std::map<PackedVertex,VertexIndex>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() )
	{
		return false;
	}
	else
	{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<Vector3> & in_vertices,
	std::vector<Vector2> & in_uvs,
	std::vector<Vector3> & in_normals,

	std::vector<VertexIndex> & out_indices,
	std::vector<Vector3> & out_vertices,
	std::vector<Vector2> & out_uvs,
	std::vector<Vector3> & out_normals)
{
	std::map<PackedVertex,VertexIndex> VertexToOutIndex;

	// For each input vertex
	for (uint32 i=0; i<in_vertices.size(); i++ )
	{
		PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};		

		// Try to find a similar vertex in out_XXXX
		VertexIndex index;
		bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);

		if ( found )
		{ 
			// A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
		}
		else
		{ 
			// If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			VertexIndex newindex = (VertexIndex)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}
}

ResourceManager::ResourceManager(void)
{
}


ResourceManager::~ResourceManager(void)
{
	for(uint32 i = 0; i < m_metaMeshList.size(); ++i)
	{
		delete m_metaMeshList[i];
	}
	m_metaMeshList.clear();

	for(uint32 i = 0; i < m_materialList.size(); ++i)
	{
		delete m_materialList[i];
	}
	m_materialList.clear();

	for(uint32 i = 0; i < m_textureList.size(); ++i)
	{
		delete m_textureList[i];
	}
	m_textureList.clear();
}

ResourceManager* ResourceManager::Instance()
{
	static ResourceManager me;

	return &me;
}

void 
ResourceManager::Init()
{

}

void 
ResourceManager::GraphicUpdate()
{
	for(uint32 i = 0; i < m_textureList.size(); ++i)
	{
		if(m_textureList[i]->bNeedUpdate)
		{
			UpdateTexture(m_textureList[i]);
		}
	}
}

MetaMesh* 
ResourceManager::GetMetaMesh(const char* pszName)
{
	for(uint32 i = 0; i < m_metaMeshList.size(); ++i)
	{
		if(StringHelper::Equals(m_metaMeshList[i]->pszName,pszName))
			return m_metaMeshList[i];
	}

	//assert(!"META MESH NOT FOUND");
	return nullptr;
}

MetaCollisionMesh*
ResourceManager::GetMetaCollisionMesh(const char* pszName)
{
	for(uint32 i = 0; i < m_metaCollisionMeshList.size(); ++i)
	{
		if(StringHelper::Equals(m_metaCollisionMeshList[i]->pszName,pszName))
			return m_metaCollisionMeshList[i];
	}

	//assert(!"META COLLISION MESH NOT FOUND");
	return nullptr;
}

Material* 
ResourceManager::GetMaterial(uint32 uIndex)
{
	if(uIndex >= m_materialList.size())
	{
		assert(!"WRONG INDEX");
		return nullptr;
	}

	return m_materialList[uIndex];
}

Material* 
ResourceManager::GetMaterial(const char* pszName)
{
	for(uint32 i = 0; i < m_materialList.size(); ++i)
	{
		if(StringHelper::Equals(m_materialList[i]->pszName,pszName))
			return m_materialList[i];
	}

	assert(!"MATERIAL NOT FOUND");
	return nullptr;
}

int32
ResourceManager::GetMaterialIndex(const char* pszName)
{
	for(uint32 i = 0; i < m_materialList.size(); ++i)
	{
		if(StringHelper::Equals(m_materialList[i]->pszName,pszName))
			return i;
	}

	assert(!"MATERIAL NOT FOUND");
	return -1;
}

Texture*
ResourceManager::GetTexture(uint32 uIndex)
{
	if(uIndex >= m_textureList.size())
	{
		assert(!"WRONG INDEX");
		return nullptr;
	}

	return m_textureList[uIndex];
}

Texture*
ResourceManager::GetTexture(const char* pszName)
{
	for(uint32 i = 0; i < m_textureList.size(); ++i)
	{
		if(StringHelper::Equals(m_textureList[i]->pszName,pszName))
			return m_textureList[i];
	}

	//assert(!"TEXTURE NOT FOUND");
	return nullptr;
}

int32
ResourceManager::GetTextureIndex(const char* pszName)
{
	for(uint32 i = 0; i < m_textureList.size(); ++i)
	{
		if(StringHelper::Equals(m_textureList[i]->pszName,pszName))
			return i;
	}

	assert(!"TEXTURE NOT FOUND");
	return -1;
}

Material* 
ResourceManager::CreateMaterial(const char* pszName)
{
	Material * pMaterial = new Material();
	pMaterial->pszName = pszName;
	pMaterial->vDiffuseColour = Vector3(1.0f,1.0f,1.0f);
	pMaterial->vSpecularColour = Vector3(1.0f,1.0f,1.0f);
	pMaterial->fShininess = 16;
	pMaterial->bMetallic = 1;
	pMaterial->nProgramIndex = -1;
	pMaterial->nDiffuseTextureIndex = -1;
	pMaterial->nSpecularTextureIndex = -1;
	pMaterial->nNormalTextureIndex = -1;
	m_materialList.push_back(pMaterial);

	return pMaterial;
}

bool 
ResourceManager::LoadMesh(const char * pszMeshPath, const char* pszMeshName, bool bInvertUV /* = false*/)
{
	//check if already loaded
	if(GetMetaMesh(pszMeshName) != nullptr)
	{
		assert(!"already loaded");
		return false;
	}

	FILE* file = nullptr;
	fopen_s(&file,pszMeshPath, "r");
	if( file == nullptr )
	{
		Debug::Log("Impossible to open the file %s!\n",pszMeshPath);
		return false;
	}

	std::vector<Vector3> temp_vertices; 
	std::vector<Vector2> temp_uvs;
	std::vector<Vector3> temp_normals;
	std::vector<uint32> vertexIndices, uvIndices, normalIndices;
 
	while( true )
	{
		char lineHeader[LINE_CBUFFER_SIZE];
		int res = fscanf_s(file, "%s", lineHeader,LINE_CBUFFER_SIZE);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			Vector3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if(strcmp( lineHeader, "vt" ) == 0)
		{
			Vector2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			if(bInvertUV)
				uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			Vector3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z); 
			temp_normals.push_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			std::string vertex1, vertex2, vertex3;
			uint32 vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				Debug::Log("face format wrong in file %s\n",pszMeshPath);
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else
		{
			//not care
			char buffer[1024];
			fgets(buffer, 1024, file);
		}
	}

	fclose(file);	//file read done

	MetaMesh* pMetaMesh = new MetaMesh();
	pMetaMesh->pszName = pszMeshName;
	m_metaMeshList.push_back(pMetaMesh);

	std::vector<Vector3> temp_vertices2; 
	std::vector<Vector2> temp_uvs2;
	std::vector<Vector3> temp_normals2;

	//recreate based on indices
	for(uint32 i=0; i<vertexIndices.size(); i++ )
	{
		// Get the indices of its attributes
		uint32 vertexIndex = vertexIndices[i];
		uint32 uvIndex = uvIndices[i];
		uint32 normalIndex = normalIndices[i];
		
		// Put the attributes in buffers
		temp_vertices2.push_back(temp_vertices[ vertexIndex-1 ]);
		temp_uvs2.push_back(temp_uvs[ uvIndex-1 ]);
		temp_normals2.push_back(temp_normals[ normalIndex-1 ]);	
	}

	indexVBO(temp_vertices2, temp_uvs2, temp_normals2, pMetaMesh->indices, pMetaMesh->vertices, pMetaMesh->uvs, pMetaMesh->normals);

	return true;
}

bool 
ResourceManager::LoadCollisionMesh(const char * pszMeshPath, const char* pszMeshName)
{
	//check if already loaded
	if(GetMetaCollisionMesh(pszMeshName) != nullptr)
	{
		assert(!"already loaded");
		return false;
	}

	FILE * file = nullptr;
	fopen_s(&file,pszMeshPath, "r");
	if( file == nullptr )
	{
		Debug::Log("Impossible to open the file %s!\n",pszMeshPath);
		return false;
	}

	std::vector<Vector3> temp_vertices; 
	std::vector<uint32> vertexIndices;
 
	while( true )
	{
		char lineHeader[LINE_CBUFFER_SIZE];
		int res = fscanf_s(file, "%s", lineHeader,LINE_CBUFFER_SIZE);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			Vector3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			std::string vertex1, vertex2, vertex3;
			uint32 vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				matches = fscanf_s(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

				if(matches != 3)
				{
					Debug::Log("face format wrong in file %s\n",pszMeshPath);
					fclose(file);
					return false;
				}
			}
			//.obj index start from 1, so -1
			vertexIndices.push_back(vertexIndex[0]-1);
			vertexIndices.push_back(vertexIndex[1]-1);
			vertexIndices.push_back(vertexIndex[2]-1);
		}
		else
		{
			//not care
			char buffer[1024];
			fgets(buffer, 1024, file);
		}
	}

	fclose(file);	//file read done

	MetaCollisionMesh* pMetaCollisionMesh = new MetaCollisionMesh();
	pMetaCollisionMesh->pszName = pszMeshName;
	pMetaCollisionMesh->vertices.reserve(temp_vertices.size());
	pMetaCollisionMesh->indices.reserve(vertexIndices.size());
	pMetaCollisionMesh->vertices.insert(pMetaCollisionMesh->vertices.begin(), temp_vertices.begin(),temp_vertices.end());
	pMetaCollisionMesh->indices.insert(pMetaCollisionMesh->indices.begin(), vertexIndices.begin(), vertexIndices.end());
	m_metaCollisionMeshList.push_back(pMetaCollisionMesh);

	float32 fMaxD = 0;
	float32 fDistance = 0;
	Vector3 vOrigin(0.0f, 0.0f,0.0f);
	Vector3 *vPoint = nullptr;
	for(uint32 i = 0; i < pMetaCollisionMesh->vertices.size(); ++i)
	{
		vPoint = &pMetaCollisionMesh->vertices[i];

		fDistance = MFD_Distance(vOrigin, (*vPoint));
		if(fDistance > fMaxD)
			fMaxD = fDistance;

		if(vPoint->x < pMetaCollisionMesh->fMinX)
			pMetaCollisionMesh->fMinX = vPoint->x;

		if(vPoint->x > pMetaCollisionMesh->fMaxX)
			pMetaCollisionMesh->fMaxX = vPoint->x;

		if(vPoint->y < pMetaCollisionMesh->fMinY)
			pMetaCollisionMesh->fMinY = vPoint->y;

		if(vPoint->y > pMetaCollisionMesh->fMaxY)
			pMetaCollisionMesh->fMaxY = vPoint->y;

		if(vPoint->z < pMetaCollisionMesh->fMinZ)
			pMetaCollisionMesh->fMinZ = vPoint->z;

		if(vPoint->z > pMetaCollisionMesh->fMaxZ)
			pMetaCollisionMesh->fMaxZ = vPoint->z;
	}

	pMetaCollisionMesh->fMaxRange = fMaxD;
	pMetaCollisionMesh->fLengthX = fabsf(pMetaCollisionMesh->fMaxX - pMetaCollisionMesh->fMinX);
	pMetaCollisionMesh->fLengthY = fabsf(pMetaCollisionMesh->fMaxY - pMetaCollisionMesh->fMinY);
	pMetaCollisionMesh->fLengthZ = fabsf(pMetaCollisionMesh->fMaxZ - pMetaCollisionMesh->fMinZ);

	return true;
}

RefCountPtr<Mesh>
ResourceManager::CreateMesh(const char* pszMeshName)
{
	MetaMesh* pMetaMesh = GetMetaMesh(pszMeshName);
	if(pMetaMesh == nullptr)
		return nullptr;

	RefCountPtr<Mesh> pMesh = new Mesh();


	//copy the data
	int32 nNumVertices = pMetaMesh->vertices.size();
	int32 nNumIndices = pMetaMesh->indices.size();	
	
	pMesh->m_vertices.reserve(nNumVertices);
	pMesh->m_normals.reserve(nNumVertices);
	pMesh->m_uvs.reserve(nNumVertices);
	pMesh->m_indices.reserve(nNumIndices);
	pMesh->m_vertices.insert(pMesh->m_vertices.begin(),pMetaMesh->vertices.begin(),pMetaMesh->vertices.end());
	pMesh->m_normals.insert(pMesh->m_normals.begin(),pMetaMesh->normals.begin(),pMetaMesh->normals.end());
	pMesh->m_uvs.insert(pMesh->m_uvs.begin(),pMetaMesh->uvs.begin(),pMetaMesh->uvs.end());
	pMesh->m_indices.insert(pMesh->m_indices.begin(),pMetaMesh->indices.begin(),pMetaMesh->indices.end());

	pMesh->CreatePhysicsGeometryData();
	pMesh->CreateBuffers();

	return pMesh;
}

RefCountPtr<CollisionMesh> 
ResourceManager::CreateCollisionMesh(const char* pszMeshName)
{
	MetaCollisionMesh* pMetaColliderMesh = GetMetaCollisionMesh(pszMeshName);
	if(pMetaColliderMesh == nullptr)
		return nullptr;

	RefCountPtr<CollisionMesh> pColliderMesh = new CollisionMesh();
	//copy the data
	int32 nNumVertices = pMetaColliderMesh->vertices.size();
	int32 nNumIndices = pMetaColliderMesh->indices.size();	
	
	pColliderMesh->m_vertices.reserve(nNumVertices);
	pColliderMesh->m_indices.reserve(nNumIndices);
	pColliderMesh->m_vertices.insert(pColliderMesh->m_vertices.begin(),pMetaColliderMesh->vertices.begin(),pMetaColliderMesh->vertices.end());
	pColliderMesh->m_indices.insert(pColliderMesh->m_indices.begin(),pMetaColliderMesh->indices.begin(),pMetaColliderMesh->indices.end());

	pColliderMesh->CreatePhysicsGeometryData();
	pColliderMesh->m_fRange = pMetaColliderMesh->fMaxRange;
	pColliderMesh->m_fMinX = pMetaColliderMesh->fMinX;
	pColliderMesh->m_fMaxX = pMetaColliderMesh->fMaxX;
	pColliderMesh->m_fMinY = pMetaColliderMesh->fMinY;
	pColliderMesh->m_fMaxY = pMetaColliderMesh->fMaxY;
	pColliderMesh->m_fMinZ = pMetaColliderMesh->fMinZ;
	pColliderMesh->m_fMaxZ = pMetaColliderMesh->fMaxZ;
	pColliderMesh->m_fLengthX = pMetaColliderMesh->fLengthX;
	pColliderMesh->m_fLengthY = pMetaColliderMesh->fLengthY;
	pColliderMesh->m_fLengthZ = pMetaColliderMesh->fLengthZ;

	return pColliderMesh;
}

void 
ResourceManager::UpdateTexture(Texture* pTexture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pTexture->uGLID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->uWidth, pTexture->uHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(pTexture->metaData[0]));


	pTexture->bNeedUpdate = false;
}

bool 
ResourceManager::CreateTexture(const char* pszResourceName,uint32 uWidth, uint32 uHeight)
{
	if(GetTexture(pszResourceName) != nullptr)
	{
		assert(!"already loaded");
		return false;
	}

	Texture *pTexture = new Texture();
	pTexture->uWidth = uWidth;
	pTexture->uHeight = uHeight;
	pTexture->metaData.resize(uWidth * uHeight * 4);
	pTexture->pszName = pszResourceName;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,&(pTexture->uGLID));
	glBindTexture(GL_TEXTURE_2D, pTexture->uGLID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->uWidth, pTexture->uHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(pTexture->metaData[0]));
			
	m_textureList.push_back(pTexture);
	return true;
}

bool 
ResourceManager::LoadTexture(const char* pszFN,  const char* pszResourceName)
{
	if(StringHelper::EndWith(pszFN,".png")
		|| StringHelper::EndWith(pszFN,".PNG"))
	{
		if(GetTexture(pszResourceName) != nullptr)
		{
			assert(!"already loaded");
			return false;
		}

		//it's a png file
		Texture *pTexture = new Texture();
		if(lodepng::decode(pTexture->metaData, pTexture->uWidth, pTexture->uHeight, pszFN) == 0 
			&& pTexture->metaData.size() > 0
			&& pTexture->uWidth != 0
			&& pTexture->uHeight != 0)
		{
			//no mipmap
			glEnable(GL_TEXTURE_2D);
			glGenTextures(1,&(pTexture->uGLID));
			glBindTexture(GL_TEXTURE_2D, pTexture->uGLID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->uWidth, pTexture->uHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(pTexture->metaData[0]));
			
			pTexture->pszName = pszResourceName;
			m_textureList.push_back(pTexture);
			return true;
		}
		else
		{
			Debug::Log("FAIL TO LOAD PNG FILE AS TEXTURE, %s",pszFN);
			delete pTexture;
			return false;
		}
	}
	else
	{
		assert(!"UNSUPPORTED FILE TYPE");
		return false;
	}
}


GLuint 
ResourceManager::LoadShader(const char * pszVertexFN, const char * pszFragmentFN, const char * pszGeometryFN)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(pszVertexFN, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		Debug::Log("ERROR: Fail to open %s.", pszVertexFN);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(pszFragmentFN, std::ios::in);
	if(FragmentShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
		{
			FragmentShaderCode += "\n" + Line;
		}
		FragmentShaderStream.close();
	}
	else
	{
		Debug::Log("ERROR: Fail to open %s.", pszFragmentFN);
		return 0;
	}

	// Read the Geometry Shader code from the file, if there is one
	std::string GeometryShaderCode;
	if(pszGeometryFN != nullptr)
	{
		std::ifstream GeometryShaderStream(pszGeometryFN, std::ios::in);
		if(GeometryShaderStream.is_open())
		{
			std::string Line = "";
			while(getline(GeometryShaderStream, Line))
			{
				GeometryShaderCode += "\n" + Line;
			}
			GeometryShaderStream.close();
		}
		else
		{
			Debug::Log("ERROR: Fail to open %s.", pszGeometryFN);
			return 0;//can I ignore Geometry part, if it failed?
		}
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	Debug::Log("Compiling shader : %s\n", pszVertexFN);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		Debug::Log("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	Debug::Log("Compiling shader : %s\n", pszFragmentFN);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		Debug::Log("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Compile Geometry Shader, if there is one
	if(pszGeometryFN != nullptr)
	{
		Debug::Log("Compiling shader : %s\n", pszGeometryFN);
		char const * GeometrySourcePointer = GeometryShaderCode.c_str();
		glShaderSource(GeometryShaderID, 1, &GeometrySourcePointer , nullptr);
		glCompileShader(GeometryShaderID);

		// Check Geometry Shader
		glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 )
		{
			std::vector<char> GeometryShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(GeometryShaderID, InfoLogLength, nullptr, &GeometryShaderErrorMessage[0]);
			Debug::Log("%s\n", &GeometryShaderErrorMessage[0]);
		}

	}

	// Link the program
	Debug::Log("Linking program\n");
	GLuint ProgramID = glCreateProgram();

	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	if(pszGeometryFN != nullptr)
		glAttachShader(ProgramID, GeometryShaderID);

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		Debug::Log("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(GeometryShaderID);

	return ProgramID;
}