/*
* Used to generate random object for launcher
*
* @author: Kai Yang
*/

#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <vector>

class GameObject;
class Scene;

class ObjectPool
{
public:
	ObjectPool(void);
	~ObjectPool(void);

	void Init(Scene* pScene, uint32 uMaxNumber);

	GameObject* GetNextObject();

private:
	uint32 m_uMaxNumber;
	uint32 m_uCurrentIndex;
	Scene* m_pScene;

	std::vector<GameObject*> m_objects;

};

#endif