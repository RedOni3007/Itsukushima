/*
* mesh + material + other data for rendering
* todo:write a better description, it's hard
*
* @author: Kai Yang
*/

#ifndef MODEL_H
#define MODEL_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>
#include <Resource/Mesh.h>
#include <Render/RenderManager_Defines.h>

class Model : public GameObjectComponent
{
public:
	Model();
	virtual ~Model();

public:
	void SetMesh(RefCountPtr<Mesh> pMesh);
	Mesh* GetMesh();//weak link

	void SetMaterialIndex(uint32 uMaterialIndex);
	void SetMaterial(const char* pszName);
	int32 GetMaterialIndex();

	std::vector<Vector3>* GetVerticesCache();

	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	static const char* ClassName();

	void SetTransparency(float32 fTransparency);
	float32 GetTransparency();

	void SetTintColor(Vector4 vColor);
	Vector4 GetTintColor();

	bool IsRenderPassEnabled(RenderPass nPass);

	void SetRenderPass(RenderPass nPass, bool bEnable);

private:
	bool m_bVerticesCached;
	std::vector<Vector3> m_meshVerticesCache;

	RefCountPtr<Mesh> m_pMesh;
	uint32 m_uMaterialIndex;
	
	//contorl color over material, so same material object can be in different color
	Vector4 m_vTintColor;//alpha only working in transparent pass

	int32 m_nRenderPassFlags;

	void RecalculteVerticesCache();
};

#endif
