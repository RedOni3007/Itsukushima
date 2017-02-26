/*
* CollisionManager
* To be simple: Everything about collision, from detection to response
* todo: to be or not to be, continually improve
*
* @author: Kai Yang
*/


#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <Core/CoreHeaders.h>
#include <Physics/PhysicsStructs.h>
#include <vector>

#define MaxConactPoints 1//just one, so far

class GameObject;
class Collider;
class Scene;

struct ContactPoint
{
	Vector3 vPos;
	Vector3 vNormal;
};

struct PlaneHitInfo
{
	Plane sPlane;
	GameObject* pObject;
	std::vector<Vector3> touchedPostions;
	std::vector<float32> touchedPostionsT;
	uint32 uTouchPostionCount;

	std::vector<uint32> ObjectFaces;
	std::vector<uint32> ObjectFaceSingleSides;//which vertex is on the other side of plane
	uint32 uObjectFacesCount;


	PlaneHitInfo()
	{
		pObject = nullptr;
		uTouchPostionCount = 0;
		uObjectFacesCount = 0;
	}

	void clear()
	{
		uTouchPostionCount = 0;
		uObjectFacesCount = 0;
	}
};

struct CollisionInfo
{
	GameObject* pObject1;
	GameObject* pObject2;
	std::vector<Vector3> touchedPostions;
	uint32 uTouchPostionCount;

	ContactPoint contactPoints[MaxConactPoints];

	std::vector<uint32> Object1Faces;
	std::vector<uint32> Object2Faces;
	uint32 uObject1FacesCount;
	uint32 uObject2FacesCount;

	bool bCollided;//for outside scene

	CollisionInfo()
	{
		pObject1 = nullptr;
		pObject2 = nullptr;
		bCollided = false;
		uTouchPostionCount = 0;
		uObject1FacesCount = 0;
		uObject2FacesCount = 0;
	}

	void clear()
	{
		bCollided = false;
		uTouchPostionCount = 0;
		uObject1FacesCount = 0;
		uObject2FacesCount = 0;
	}
};

struct CollisionInfoCollection
{
	std::vector<CollisionInfo> collsionInfoList;
	uint32 uCount;

	CollisionInfoCollection()
	{
		uCount = 0;
	}
};

//non-singlton, every screen should have its own CollisionManger instance
class CollisionManager
{
public:
	CollisionManager(void);
	~CollisionManager(void);

public:
	static CollisionManager* Instance();

	void CheckCollision(Scene* pScene);
	void PrepareCollisionInfo(CollisionInfo* pCollisionInfo);

	void ProcessCollision(CollisionInfo *pCollisionInfo);

	void SeparateObject(CollisionInfo *pCollisionInfo);

	bool IsColliderCollided(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo);
	bool IsRayColliderCollided(Vector3& vRayPos, Vector3& vRayDir, float32 fMaxRayLength, GameObject* pObject, Vector3& pOutHitPos);

	//there won't be plane primity shape, this is for my cut feature
	bool IsPlaneColliderCollided(GameObject* pObject, PlaneHitInfo& outInfo);
	bool IsPlaneMeshCollided(GameObject* pObject, PlaneHitInfo& outInfo);

	bool IsMeshCollided(GameObject* pObject1, GameObject* pObject2);
	//multi thread
	bool IsMeshCollided_MT(GameObject* pObject1, GameObject* pObject2);

private:
	bool MeshColliderToMeshCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo);
	bool SphereColliderToSphereCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo);
	bool SphereColliderToMeshCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo, bool bSwapObjects);//object1 should be the sphere collider


};

#endif