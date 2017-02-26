/*
* mesh class
*
* @author: Kai Yang
*/

#ifndef MESH_H
#define MESH_H

#include <Core/CoreHeaders.h>
#include <vector>
#include <Resource/ResourceStructDefines.h>

#include <Core/GLEW_GLFW.h>

class CollisionMesh;

class Mesh : public RefCountBase
{
	friend class ResourceManager;
	friend class CollisionManager;

public:
	Mesh(void);
	~Mesh(void);

	GLuint GetVAO();
	GLuint GetIndicesBuffer();

	uint32 GetVertexCount();
	uint32 GetIndexCount();
	uint32 GetFaceCount();

	std::vector<Vector3>* GetVertices();
	std::vector<Face>* GetFaces();

	std::vector<Vector3>* GetNormals();
	std::vector<Vector2>* GetUVs();
	std::vector<VertexIndex>* GetIndices();

	void RefreshMesh();//only call this function on OpenGL thread
	void CreateBuffers();//only call this function on OpenGL thread

	Vector3 ResetVerticesOnMiddlePoint();//return middle point of vertices

	void CreatePhysicsGeometryData();

	CollisionMesh* CreateCollisionMesh();

private:
	void DeleteBuffers();

private:
	//data for feed graphic card
	std::vector<Vector3> m_vertices;
	std::vector<Vector2> m_uvs;
	std::vector<Vector3> m_normals;
	std::vector<VertexIndex> m_indices;//65535 should be enough 

	//data for physics and geometry calculation
	std::vector<Face> m_Faces;

	/*
	*  mesh will be changed in real-time
	*  so every mesh get a VAO, it's slow
	*/
	GLuint m_VAO_ID;
	GLuint m_VerticesBuffer_ID;
	GLuint m_UVBuffer_ID;
	GLuint m_NormalBuffer_ID;
	GLuint m_IndicesBuffer_ID;
};

#endif