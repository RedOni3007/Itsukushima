#include <Physics/Collider.h>
#include <Math/MathHelper.h>

Collider::Collider()
{
	m_fRange = 0;
	m_fMetaRange = 0;
	m_pColliderMesh = nullptr;
}


Collider::~Collider()
{
	m_pColliderMesh = nullptr;
}

//auto change type to ColliderType::Mesh
void 
Collider::SetMesh(RefCountPtr<CollisionMesh> pMesh)
{
	m_pColliderMesh = pMesh;

	//resize the memory
	m_meshVerticesCache.clear();
	m_meshVerticesCache.reserve(m_pColliderMesh->GetVertexCount());
	const std::vector<Vector3> *pVertices = m_pColliderMesh->GetVertices();
	m_meshVerticesCache.insert(m_meshVerticesCache.begin(),pVertices->begin(),pVertices->end());

	m_fMetaRange = m_fRange = pMesh->GetRange();

	m_eType = ColliderType::Mesh;
}

void 
Collider::LogicUpdate()
{
	if(m_bActive == false)
		return;

	Vector3 vScale = m_pGameObject->GetScale();
	m_fRange = m_fMetaRange * MFD_Max(MFD_Max(fabsf(vScale.x), fabsf(vScale.y)), fabsf(vScale.z));
	m_bVerticesCached = false;
}

void
Collider::GraphicUpdate()
{

}

const char*
Collider::GetName()
{
	return "Collider";
}

const char* 
Collider::ClassName()
{
	return "Collider";
}

std::vector<Vector3>* 
Collider::GetVerticesCache()
{
	if(m_bVerticesCached == false)
	{
		RecalculteVerticesCache();
	}

	return &m_meshVerticesCache;
}


void
Collider::RecalculteVerticesCache()
{
	if(m_bActive == false)
		return;

	if(m_eType == ColliderType::Sphere)
		return;

	if(m_bVerticesCached)
		return;

	const std::vector<Vector3> &pVertices = *m_pColliderMesh->GetVertices();
	uint32 uVertexCount = m_meshVerticesCache.size();
	Matrix44 &mWorldMat = *m_pGameObject->GetTranformMat();

	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		m_meshVerticesCache[i] = glm::vec3(mWorldMat * Vector4(pVertices[i],1.0f));
	}

	uint32 uFaceCount =  m_pColliderMesh->GetFaceCount();
	std::vector<Face> &faces = *m_pColliderMesh->GetFaces();
	Face *pFace  = nullptr;
	Vector3 vNormal;
	for(uint32 i = 0; i < uFaceCount;++i)
	{
		pFace = &faces[i];
		//if use world mat to tranform the normal it could be very wrong because the build-up of tiny float precision errors
		//so calculate the normal again according to the vectors
		vNormal = MFD_Cross(m_meshVerticesCache[pFace->v[1]]-m_meshVerticesCache[pFace->v[0]],m_meshVerticesCache[pFace->v[2]] - m_meshVerticesCache[pFace->v[0]]);
		if(MathHelper::IsZeroVector(vNormal) == false)
		{
			pFace->normal = MFD_Normalize(vNormal);
		}
		else
		{
			//assert(!"Something wrong");
			pFace->normal = Vector3(0,1,0);
		}
	}

	m_bVerticesCached = true;
}

float32 
Collider::GetRange()
{
	return m_fRange;
}

void 
Collider::SetRange(float32 fRange)
{
	m_fMetaRange = fRange;
}

void 
Collider::SetType(ColliderType eType, float32 fRange/*= 0*/)//range only for sphere
{
	m_eType = eType;
	if(m_eType == ColliderType::Sphere)
	{
		SetRange(fRange);
		m_fRange = fRange;//in case, this is called after update 
	}
}
	
ColliderType 
Collider::GetType()
{
	return m_eType;
}

CollisionMesh* 
Collider::GetCollisionMesh()
{
	return m_pColliderMesh.get();
}

