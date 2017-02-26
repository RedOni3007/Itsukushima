#include "CollisionMesh.h"
#include <Resource/ResourceManager.h>

CollisionMesh::CollisionMesh(void)
{
	m_fRange = 0;
	m_fMinX = m_fMinY = m_fMinZ = 0;
	m_fMaxX = m_fMaxY = m_fMaxZ = 0;
	m_fLengthX = m_fLengthY = m_fLengthZ;
}


CollisionMesh::~CollisionMesh(void)
{
}

float32 
CollisionMesh::GetRange()
{
	return m_fRange;
}

float32 
CollisionMesh::GetLengthX()
{
	return m_fLengthX;
}

float32 
CollisionMesh::GetLengthY()
{
	return m_fLengthY;
}

float32 
CollisionMesh::GetLengthZ()
{
	return m_fLengthZ;
}

uint32 
CollisionMesh::GetVertexCount()
{
	return m_vertices.size();
}

uint32 
CollisionMesh::GetIndexCount()
{
	return m_indices.size();
}

uint32 
CollisionMesh::GetFaceCount()
{
	return m_Faces.size();
}

std::vector<Face>*
CollisionMesh::GetFaces()
{
	return &m_Faces;
}

std::vector<Vector3>*
CollisionMesh::GetVertices()
{
	return &m_vertices;
}

std::vector<VertexIndex>* 
CollisionMesh::GetIndices()
{
	return &m_indices;
}

void 
CollisionMesh::CreatePhysicsGeometryData()
{
	//uint32 uIndex = 0;
	m_Faces.reserve(m_indices.size()/3);
	for(uint32 i = 0; i < m_indices.size(); i += 3)
	{
		m_Faces.push_back(Face(m_indices[i],m_indices[i+1],m_indices[i+2]));
		//m_Faces[uIndex].index = uIndex;
		//++uIndex;
	}

	/* no need for adj faces at this moment
	//super super expensive T_T
	Face* pCurrentFace = nullptr;
	Face* pTargetFace = nullptr;
	uint32 uCurrentVertex;
	uint32 uHitCount = 0;
	for(uint32 i = 0; i < m_Faces.size(); ++i)
	{
		pCurrentFace= &m_Faces[i];
		for(uint32 j = (i + 1); j < m_Faces.size(); ++j)
		{
			pTargetFace =  &m_Faces[j];
			if(pCurrentFace == pTargetFace)
				continue;

			uHitCount = 0;
			for(int k = 0; k < 3; ++k)
			{
				uCurrentVertex = pCurrentFace->v[k];
				for(int l = 0; l < 3; ++l)
				{
					if(uCurrentVertex == pTargetFace->v[l])
					{
						++uHitCount;
						break;
					}
				}

				if(uHitCount == 2)
					break;
			}

			if(uHitCount == 2)
			{
				pCurrentFace->adjFaces.push_back(j);
				pTargetFace->adjFaces.push_back(i);
			}
		}
	}
	*/
}

Vector3 
CollisionMesh::ResetVerticesOnMiddlePoint()
{
	m_fMinX = m_fMinY = m_fMinZ = FLT_MAX;
	m_fMaxX = m_fMaxY = m_fMaxZ = -FLT_MAX;

	Vector3 vVertexMiddlePoint;

	uint32 uVertexCount = m_vertices.size();
	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		if(m_vertices[i].x < m_fMinX)
			m_fMinX = m_vertices[i].x;

		if(m_vertices[i].y < m_fMinY)
			m_fMinY = m_vertices[i].y;

		if(m_vertices[i].z < m_fMinZ)
			m_fMinZ = m_vertices[i].z;

		if(m_vertices[i].x > m_fMaxX)
			m_fMaxX = m_vertices[i].x;

		if(m_vertices[i].y > m_fMaxY)
			m_fMaxY = m_vertices[i].y;

		if(m_vertices[i].z > m_fMaxZ)
			m_fMaxZ = m_vertices[i].z;
	}

	Vector3 vMinxyz(m_fMinX,m_fMinY,m_fMinZ);
	Vector3 vMaxxyz(m_fMaxX,m_fMaxY,m_fMaxZ);
	vVertexMiddlePoint = (vMinxyz + vMaxxyz) * 0.5f;
	Vector3 vOrigin(0.0f, 0.0f,0.0f);
	float32 fMaxD = 0;
	float32 fDistance = 0;
	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		m_vertices[i] -= vVertexMiddlePoint;

		fDistance = MFD_Distance(vOrigin, m_vertices[i]);
		if(fDistance > fMaxD)
			fMaxD = fDistance;

	}
	m_fMinX -= vVertexMiddlePoint.x;
	m_fMinY -= vVertexMiddlePoint.y;
	m_fMinZ -= vVertexMiddlePoint.z;
	m_fMaxX -= vVertexMiddlePoint.x;
	m_fMaxY -= vVertexMiddlePoint.y;
	m_fMaxZ -= vVertexMiddlePoint.z;

	m_fLengthX = fabsf(m_fMaxX - m_fMinX);
	m_fLengthY = fabsf(m_fMaxY - m_fMinY);
	m_fLengthZ = fabsf(m_fMaxZ - m_fMinZ);

	m_fRange = fMaxD;

	return vVertexMiddlePoint;
}

float32 
CollisionMesh::GetRoughVolume()
{
	return m_fLengthX * m_fLengthY * m_fLengthZ;
}