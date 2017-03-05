/**
* Hold and maintain GameObjects, might rename to Layer, and create a new true scene class
*
* @author: Kai Yang
**/

#ifndef SCENE_H
#define SCENE_H

#include <Core/CoreHeaders.h>

#include <vector>
#include <list>

class GameObject;
struct CollisionInfo;
struct CollisionInfoCollection;

class Scene
{
public:
	Scene();
	~Scene();

	void InitCollisionCollection(uint32 uCollisionSize, uint32 uTouchPositionSize);

	GameObject* CreateGameObject(const char* pszName);
	
	void AddObject(GameObject& object);

	//return false, if the object does not belong to this scene
	bool RemoveObject(GameObject* pObject);

	int32 GetObjectCount();

	GameObject* GetGameObject(uint32 uIndex);
	GameObject* GetGameObject(const char* pszName);

	void PreLogicUpdate();
	void LogicUpdate();
	void PostLogicUpdate();
	void GraphicUpdate();

	void NetworkUpdate();//a special update, which only is called from NetworkManager

	std::list<GameObject*>* GetObjectList();

	CollisionInfoCollection* GetCollisionInfoCollection();

	GameObject* RayHit(Vector3& vRayPos, Vector3& vRayDir, float32 fMaxLength,Vector3& vOutHitPos);

	void GetComponents(const char* pszCompName, std::list<void *>& pList);

	Vector3		GetGlobalLightDir();
	void		SetGlobalLightDir(Vector3 vDir);

private:
	std::list<GameObject*> m_gameObjectList;//todo:change to linked list
	CollisionInfoCollection* m_pCollsionInfoCollection;

	Vector3 m_vGlobalLightDir;

};

#endif