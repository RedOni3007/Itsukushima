/*
* Collidision Mesh
*
* @author: Kai Yang
*/

#ifndef COLLIDER_MESH_H
#define COLLIDER_MESH_H

#include <Core/CoreHeaders.h>
#include <vector>
#include <Resource/ResourceStructDefines.h>

class CollisionMesh : public RefCountBase
{
	friend class ResourceManager;
	friend class CollisionManager;

public:
	CollisionMesh(void);
	~CollisionMesh(void);

	uint32 GetVertexCount();
	uint32 GetIndexCount();
	uint32 GetFaceCount();

	std::vector<Vector3>* GetVertices();
	std::vector<Face>* GetFaces();
	std::vector<VertexIndex>* GetIndices();

	float32 GetRange();

	float32 GetLengthX();
	float32 GetLengthY();
	float32 GetLengthZ();

	void CreatePhysicsGeometryData();

	Vector3 ResetVerticesOnMiddlePoint();//return middle point of vertices

	//treat as box, very inaccurate
	float32 GetRoughVolume();

private:
	//data for feed graphic card
	std::vector<Vector3> m_vertices;
	std::vector<VertexIndex> m_indices;//65535 should be enough 

	//data for physics and geometry calculation
	std::vector<Face> m_Faces;

	float32 m_fRange;

	float32 m_fMinX, m_fMinY, m_fMinZ;
	float32 m_fMaxX, m_fMaxY, m_fMaxZ;
	float32 m_fLengthX, m_fLengthY, m_fLengthZ;
};

#endif