#include <Game/Scene.h>

#include <Resource/ResourceManager.h>
#include <Game/GameObject.h>
#include <Physics/CollisionManager.h>
#include <Resource/Model.h>
#include <Physics/Collider.h>
#include <Physics/RigidBody.h>
#include <Core/StringHelper.h>

#include <string>

Scene::Scene()
{
	m_pCollsionInfoCollection = nullptr;
	m_vGlobalLightDir = MFD_Normalize(Vector3(-1.0f,3.0f, -1.0f));
}


Scene::~Scene()
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		delete (*it);
	}
	m_gameObjectList.clear();

	m_gameObjectList.clear();
	
	if(m_pCollsionInfoCollection != nullptr)
	{
		delete m_pCollsionInfoCollection;
		m_pCollsionInfoCollection = nullptr;
	}
}

void 
Scene::InitCollisionCollection(uint32 uCollisionSize, uint32 uTouchPositionSize)
{
	if(m_pCollsionInfoCollection != nullptr)
	{
		delete m_pCollsionInfoCollection;
	}

	m_pCollsionInfoCollection = new CollisionInfoCollection();
	m_pCollsionInfoCollection->collsionInfoList.resize(uCollisionSize);

	for(uint32 i = 0; i < uCollisionSize; ++i)
	{
		m_pCollsionInfoCollection->collsionInfoList[i].touchedPostions.resize(uTouchPositionSize);
		m_pCollsionInfoCollection->collsionInfoList[i].Object1Faces.resize(uTouchPositionSize/2);
		m_pCollsionInfoCollection->collsionInfoList[i].Object2Faces.resize(uTouchPositionSize/2);
	}
}

GameObject* 
Scene::CreateGameObject(const char* pszName)
{
	GameObject* pGameObject = new GameObject(pszName);
	pGameObject->m_pScene = this;
	m_gameObjectList.push_back(pGameObject);

	return pGameObject;
}
	
void 
Scene::AddObject(GameObject& object)
{
	object.m_pScene = this;
	m_gameObjectList.push_back(&object);
}

bool 
Scene::RemoveObject(GameObject* pObject)
{
	bool bFound = false;
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end();)
	{
		if((*it) == pObject)
		{
			bFound = true;
			delete (*it);
			it = m_gameObjectList.erase(it);
		}
		else
		{
			if((*it)->m_pParent == pObject)
			{
				(*it)->SetParent(nullptr);			
			}
			++it;
		}
	}

	return bFound;
}

int32 
Scene::GetObjectCount()
{
	return m_gameObjectList.size();
}

GameObject* 
Scene::GetGameObject(uint32 uIndex)
{
	if(uIndex < m_gameObjectList.size())
	{
		std::list<GameObject*>::iterator it = m_gameObjectList.begin();
		std::advance(it,uIndex);

		return (*it);
	}
	else
	{
		return nullptr;
	}
}

GameObject* 
Scene::GetGameObject(const char* pszName)
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		if(StringHelper::Equals((*it)->GetName(),pszName))
			return (*it);
	}

	return nullptr;
}

void 
Scene::PreLogicUpdate()
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		(*it)->PreLogicUpdate();
	}
}

void 
Scene::LogicUpdate()
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		(*it)->LogicUpdate();
	}

	CollisionManager::Instance()->CheckCollision(this);
	
	CollisionInfo *pCollisionInfo = nullptr;
	for(uint32 i = 0; i < m_pCollsionInfoCollection->uCount; ++i)
	{
		pCollisionInfo = &m_pCollsionInfoCollection->collsionInfoList[i];
		CollisionManager::Instance()->PrepareCollisionInfo(pCollisionInfo);

		CollisionManager::Instance()->ProcessCollision(pCollisionInfo);

		//pass to object
		pCollisionInfo->pObject1->OnCollision(pCollisionInfo->pObject2, pCollisionInfo);
		pCollisionInfo->pObject2->OnCollision(pCollisionInfo->pObject1, pCollisionInfo);
	}
}

void 
Scene::PostLogicUpdate()
{
	CollisionInfo *pCollisionInfo = nullptr;
	for(uint32 i = 0; i < m_pCollsionInfoCollection->uCount; ++i)
	{
		pCollisionInfo = &m_pCollsionInfoCollection->collsionInfoList[i];
		CollisionManager::Instance()->SeparateObject(pCollisionInfo);

		//clear
		pCollisionInfo->clear();
	}
	m_pCollsionInfoCollection->uCount = 0;

	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		(*it)->PostLogicUpdate();
	}

	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end();)
	{
		if((*it)->IsDead())
		{
			delete (*it);
			it = m_gameObjectList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void 
Scene::NetworkUpdate()
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		(*it)->NetworkUpdate();
	}
}

void 
Scene::GraphicUpdate()
{
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		(*it)->GraphicUpdate();
	}
}

std::list<GameObject*>*
Scene::GetObjectList()
{
	return &m_gameObjectList;
}

CollisionInfoCollection* 
Scene::GetCollisionInfoCollection()
{
	return m_pCollsionInfoCollection;
}

GameObject* 
Scene::RayHit(Vector3& vRayPos, Vector3& vRayDir, float32 fMaxLength, Vector3& vOutHitPos)
{
	GameObject* pResult = nullptr;
	float32 fMinDis = FLT_MAX;
	float32 fDis;
	Vector3 vTemp;
	Vector3 vHitPos;

	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		if(CollisionManager::Instance()->IsRayColliderCollided(vRayPos,vRayDir,fMaxLength,*it,vHitPos))
		{
			vTemp = vRayPos - vHitPos;
			fDis = MFD_Dot(vTemp,vTemp);
			if(fDis < fMinDis)
			{
				fMinDis = fDis;
				pResult = *it;
				vOutHitPos = vHitPos;
			}
		}		
	}

	return pResult;
}

void 
Scene::GetComponents(const char* pszCompName, std::list<void *>& pList)
{
	void* pTempPtr = nullptr;
	std::list<GameObject*>::iterator it;
	for(it = m_gameObjectList.begin(); it != m_gameObjectList.end(); ++it)
	{
		pTempPtr = (void*)(*it)->GetComponent(pszCompName);
		if(pTempPtr != nullptr)
		{
			pList.push_back(pTempPtr);
		}
	}
}

Vector3		
Scene::GetGlobalLightDir()
{
	return m_vGlobalLightDir;
}
	
void		
Scene::SetGlobalLightDir(Vector3& vDir)
{
	m_vGlobalLightDir = vDir;
}