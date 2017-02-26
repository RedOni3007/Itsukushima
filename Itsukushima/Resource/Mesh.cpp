#include <Resource/Mesh.h>
#include <Resource/ResourceManager.h>
#include <Resource/CollisionMesh.h>

Mesh::Mesh(void)
{
	m_VAO_ID = 0;
	m_VerticesBuffer_ID = 0;
	m_UVBuffer_ID = 0;
	m_NormalBuffer_ID = 0;
	m_IndicesBuffer_ID = 0;
}


Mesh::~Mesh(void)
{
	DeleteBuffers();
}

void 
Mesh::DeleteBuffers()
{
	if(m_VerticesBuffer_ID != 0)
		glDeleteBuffers(1,&m_VerticesBuffer_ID);

	if(m_UVBuffer_ID != 0)
		glDeleteBuffers(1,&m_UVBuffer_ID);

	if(m_NormalBuffer_ID != 0)
		glDeleteBuffers(1,&m_NormalBuffer_ID);

	if(m_IndicesBuffer_ID != 0)
		glDeleteBuffers(1,&m_IndicesBuffer_ID);

	if(m_VAO_ID != 0)
		glDeleteVertexArrays(1,&m_VAO_ID);
}

void 
Mesh::CreatePhysicsGeometryData()
{
	//uint32 uIndex = 0;
	m_Faces.reserve(m_indices.size()/3);
	for(uint32 i = 0; i < m_indices.size(); i += 3)
	{
		m_Faces.push_back(Face(m_indices[i],m_indices[i+1],m_indices[i+2]));
		//m_Faces[uIndex].index = uIndex;
		//++uIndex;
	}

	/*no need for adj faces at this momment
	//super super expensive T_T
	Face* pCurrentFace = nullptr;
	Face* pTargetFace = nullptr;

	//test position, I prefer only the position
	Vector3* pCurrentVertex = nullptr;
	Vector3* pTargetVertex = nullptr;

	//or test index, which need pos/normal/etc are all the same
	//uint32 uCurrentVertex;
	//uint32 uTargetVertex;

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
				pCurrentVertex = &m_vertices[pCurrentFace->v[k]];
				//uCurrentVertex = pCurrentFace->v[k];
				for(int l = 0; l < 3; ++l)
				{
					pTargetVertex =  &m_vertices[ pTargetFace->v[l]];
					//uTargetVertex = pTargetFace->v[l];

					if((*pCurrentVertex) == (*pTargetVertex))
					//if(uCurrentVertex == uTargetVertex)
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

GLuint 
Mesh::GetVAO()
{
	return m_VAO_ID;
}

GLuint 
Mesh::GetIndicesBuffer()
{
	return m_IndicesBuffer_ID;
}

uint32 
Mesh::GetVertexCount()
{
	return m_vertices.size();
}

uint32 
Mesh::GetIndexCount()
{
	return m_indices.size();
}

uint32 
Mesh::GetFaceCount()
{
	return m_Faces.size();
}

std::vector<Face>*
Mesh::GetFaces()
{
	return &m_Faces;
}

std::vector<Vector3>*
Mesh::GetVertices()
{
	return &m_vertices;
}

std::vector<Vector3>* 
Mesh::GetNormals()
{
	return &m_normals;
}

std::vector<Vector2>* 
Mesh::GetUVs()
{
	return &m_uvs;
}

std::vector<VertexIndex>*
Mesh::GetIndices()
{
	return &m_indices;
}

void 
Mesh::RefreshMesh()
{
	glBindVertexArray(m_VAO_ID);
	

	glBindBuffer(GL_ARRAY_BUFFER, m_VerticesBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector3) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector2) * m_uvs.size(), &m_uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(VertexIndex) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void 
Mesh::CreateBuffers()
{
	DeleteBuffers();

	int32 nNumVertices = m_vertices.size();
	int32 nNumIndices = m_indices.size();	

	glGenVertexArrays(1, &m_VAO_ID);
	glBindVertexArray(m_VAO_ID);
	

	glGenBuffers(1, &m_VerticesBuffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VerticesBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector3) * nNumVertices, &m_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0, (void*)0);

	glGenBuffers(1, &m_NormalBuffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector3) * nNumVertices, &m_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0, (void*)0);

	glGenBuffers(1, &m_UVBuffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_UVBuffer_ID);
	glBufferData(GL_ARRAY_BUFFER,  sizeof(Vector2) * nNumVertices, &m_uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0, (void*)0);

	glGenBuffers(1, &m_IndicesBuffer_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndicesBuffer_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(VertexIndex) * nNumIndices, &m_indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

Vector3 
Mesh::ResetVerticesOnMiddlePoint()
{
	Vector3 vMinxyz(FLT_MAX,FLT_MAX,FLT_MAX);
	Vector3 vMaxxyz(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	Vector3 vVertexMiddlePoint;

	uint32 uVertexCount = m_vertices.size();
	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		if(m_vertices[i].x < vMinxyz.x)
			vMinxyz.x = m_vertices[i].x;

		if(m_vertices[i].y < vMinxyz.y)
			vMinxyz.y = m_vertices[i].y;

		if(m_vertices[i].z < vMinxyz.z)
			vMinxyz.z = m_vertices[i].z;

		if(m_vertices[i].x > vMaxxyz.x)
			vMaxxyz.x = m_vertices[i].x;

		if(m_vertices[i].y > vMaxxyz.y)
			vMaxxyz.y = m_vertices[i].y;

		if(m_vertices[i].z > vMaxxyz.z)
			vMaxxyz.z = m_vertices[i].z;
	}

	vVertexMiddlePoint = (vMinxyz + vMaxxyz) * 0.5f;
	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		m_vertices[i] -= vVertexMiddlePoint;
	}

	return vVertexMiddlePoint;
}

CollisionMesh* 
Mesh::CreateCollisionMesh()
{
	CollisionMesh* pCollisionMesh =  new CollisionMesh();

	std::vector<Vector3> *newVertices = pCollisionMesh->GetVertices();
	std::vector<VertexIndex> *newIndices= pCollisionMesh->GetIndices();
	newVertices->reserve(m_vertices.size());
	newIndices->reserve(m_indices.size());

	newVertices->insert(newVertices->begin(), m_vertices.begin(), m_vertices.end());
	newIndices->insert(newIndices->begin(), m_indices.begin(), m_indices.end());

	pCollisionMesh->CreatePhysicsGeometryData();
	pCollisionMesh->ResetVerticesOnMiddlePoint();

	return pCollisionMesh;
}