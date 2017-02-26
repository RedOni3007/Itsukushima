/*
* CollisionMesh + other data for collision
* todo:write a better description, it's hard
*
* @author: Kai Yang
*/

#ifndef COLLIDER_H
#define COLLIDER_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>
#include <Resource/CollisionMesh.h>


//only mesh should be enough, but Sphere like shape will be expensive
enum class ColliderType
{
	Mesh = 0,
	Sphere, // perfect sphere mesh is too expensive
};

class Collider : public GameObjectComponent
{
public:
	Collider();
	virtual ~Collider();

	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	static const char* ClassName();

	//auto change type to ColliderType::Mesh
	void SetMesh(RefCountPtr<CollisionMesh> pMesh);

	//should always check type before ask mesh
	CollisionMesh* GetCollisionMesh();//weak link
	std::vector<Vector3>* GetVerticesCache();

	float32 GetRange();
	void SetRange(float32 fRange);

	void SetType(ColliderType eType, float32 fRange = 0);//range only for sphere
	ColliderType GetType();

private:
	void RecalculteVerticesCache();

private:
	ColliderType m_eType;

	float32 m_fRange;
	float32 m_fMetaRange;

	RefCountPtr<CollisionMesh> m_pColliderMesh;

	bool m_bVerticesCached;
	std::vector<Vector3> m_meshVerticesCache;
};

#endif