#include "Itsukushima_include.h"
#include "TestPlane.h"

TestPlane::TestPlane()
{
}


TestPlane::~TestPlane()
{
}

void 
TestPlane::LogicUpdate()
{
	float32 fTime = Timer::Instance()->GetTime();
	float32 fX = sinf(fTime) * 3;
	float32 fZ = cosf(fTime) * 3;
	float32 fDX = 0;
	float32 fDZ = 0;
	float32 fRange = 0;
	float32 fMaxRange = 2;

	Mesh *pMesh = m_pGameObject->GetModelComponent()->GetMesh();
	std::vector<Vector3> &vertices = *pMesh->GetVertices();
	Vector3 *pV = nullptr;
	uint32 uVertexCount = vertices.size();
	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		pV = &vertices[i];
		fDX = fabsf(pV->x - fX);
		fDZ = fabsf(pV->z - fZ);
		fRange =  sqrtf(fDX * fDX + fDZ * fDZ);
		if(fRange > fMaxRange )
		{
			pV->y = sinf(pV->x + fTime) * 0.2f;
		}
		else
		{
			pV->y = glm::smoothstep(0.0f, fMaxRange, fMaxRange - fRange) * -1 + sinf(pV->x + fTime) * 0.2f;
		}
	}

	pMesh->RefreshMesh();
}

const char* 
TestPlane::GetName()
{
	return "TestPlane";
}

const char* 
TestPlane::ClassName()
{
	return "TestPlane";
}