#include <Resource/Model.h>
#include <Game/GameObject.h>
#include <Resource/ResourceManager.h>
#include <glm/ext.hpp>

Model::Model()
{
	m_pMesh = nullptr;
	m_uMaterialIndex = 0;
	m_bVerticesCached = false;
	m_nRenderPassFlags = (int32)RenderPass::GBUFFER;

	m_vTintColor = Vector4(1.0f,1.0f,1.0f,1.0f);
}


Model::~Model()
{
	m_pMesh = nullptr;
}

const char* 
Model::GetName()
{
	return "Model";
}

const char* 
Model::ClassName()
{
	return "Model";
}

void 
Model::SetMesh(RefCountPtr<Mesh> pMesh)
{
	m_pMesh = pMesh;

	//resize the memory
	m_meshVerticesCache.clear();
	m_meshVerticesCache.reserve(m_pMesh->GetVertexCount());
	const std::vector<Vector3> *pVertices = m_pMesh->GetVertices();
	m_meshVerticesCache.insert(m_meshVerticesCache.begin(),pVertices->begin(),pVertices->end());

}

Mesh*
Model::GetMesh()
{
	return m_pMesh.get();
}

void 
Model::SetMaterialIndex(uint32 uMaterialIndex)
{
	m_uMaterialIndex = uMaterialIndex;
}

void 
Model::SetMaterial(const char* pszName)
{
	m_uMaterialIndex = ResourceManager::Instance()->GetMaterialIndex(pszName);
}

int32 
Model::GetMaterialIndex()
{
	return m_uMaterialIndex;
}

std::vector<Vector3>*
Model::GetVerticesCache()
{
	if(m_bVerticesCached == false)
	{
		RecalculteVerticesCache();
	}

	return &m_meshVerticesCache;
}

void 
Model::RecalculteVerticesCache()
{
	if(m_bVerticesCached)
		return;

	const std::vector<Vector3> &pVertices = *m_pMesh->GetVertices();
	uint32 uVertexCount = m_meshVerticesCache.size();
	Matrix44 &mWorldMat = *m_pGameObject->GetTranformMat();

	for(uint32 i = 0; i < uVertexCount; ++i)
	{
		m_meshVerticesCache[i] = Vector3(mWorldMat * Vector4(pVertices[i],1.0f));
	}

	m_bVerticesCached = true;
}

void 
Model::LogicUpdate()
{
	m_bVerticesCached = false;
}

void 
Model::GraphicUpdate()
{
	
}

void 
Model::SetTransparency(float32 fTransparency)
{
	m_vTintColor.a = fTransparency;
}

float32 
Model::GetTransparency()
{
	return m_vTintColor.a;
}

void 
Model::SetTintColor(Vector4 vColor)
{
	m_vTintColor = vColor;
}

Vector4 
Model::GetTintColor()
{
	return m_vTintColor;
}

bool 
Model::IsRenderPassEnabled(RenderPass nPass)
{
	return (m_nRenderPassFlags & (int32)nPass) > 0;
}

void 
Model::SetRenderPass(RenderPass nPass, bool bEnable)
{
	if(bEnable)
	{
		m_nRenderPassFlags = m_nRenderPassFlags | (int32)nPass;
	}
	else
	{
		m_nRenderPassFlags = m_nRenderPassFlags & (0xffffffff ^ (int32)nPass);
	}
}