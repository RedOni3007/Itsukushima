#include <Game/GameObject.h>
#include <Game/GameObjectComponent.h>
#include <Core/StringHelper.h>
#include <Resource/ResourceManager.h>
#include <Resource/Model.h>
#include <Physics/Collider.h>
#include <Physics/CollisionManager.h>
#include <Physics/RigidBody.h>
#include <App/Timer.h>
#include <Network/ServerControl.h>
#include <Network/ClientControl.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define MIN_MOVED_COUNT 2

GameObject::GameObject(const char* pszName)
{
	m_pszName = new char[NAME_CBUFFER_SIZE];
	strcpy_s(m_pszName,NAME_CBUFFER_SIZE,pszName);
	m_pParent = nullptr;
	m_pModel = nullptr;
	m_pCollider = nullptr;
	m_pRigidBody = nullptr;
	m_pServerControl = nullptr;
	m_pClientControl = nullptr;
	m_pScene = nullptr;

	SetPos(glm::vec3(0.0f,0.0f,0.0f));
	SetRot(glm::vec3(0.0f,0.0f,0.0f));
	SetScale(glm::vec3(1.0f,1.0f,1.0f));
	RefreshTranformMat();

	m_uMovedCount = 0;
	m_bMoved = false;

	m_bDead = false;
	m_fDeathCD = -1.0f;
}


GameObject::~GameObject()
{
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		delete (*it);
	}
	m_componentList.clear();

	m_pParent = nullptr;
	m_pModel = nullptr;
	m_pCollider = nullptr;
	m_pRigidBody = nullptr;
	m_pServerControl = nullptr;
	m_pClientControl = nullptr;
	m_pScene = nullptr;

	delete[] m_pszName;
}

const char* 
GameObject::GetName()
{
	return m_pszName;
}

void 
GameObject::SetParent(GameObject* pParent)
{
	if(pParent != nullptr && pParent->m_pParent == this)
	{
		assert(!"not allow objects become each other's parent"); 
		m_pParent->m_pParent = nullptr;
	}

	m_pParent = pParent;
}

GameObject* 
GameObject::GetParent()
{
	return m_pParent;
}

Scene* 
GameObject::GetScene()
{
	return m_pScene;
}

bool
GameObject::AddComponent(GameObjectComponent* pComponent)
{
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		if(StringHelper::Equals((*it)->GetName(),pComponent->GetName()))
		{
			assert(!"ALREADY HAVE THIS TYPE");
			return false;
		}
	}

	pComponent->m_pGameObject = this;
	m_componentList.push_back(pComponent);
	return true;
}

GameObjectComponent*
GameObject::GetComponent(const char* pszName)
{
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		if(StringHelper::Equals((*it)->GetName(),pszName))
		{
			return *it;
		}
	}

	return nullptr;
}

void 
GameObject::PreLogicUpdate()
{
	if(m_fDeathCD > 0)
	{
		m_fDeathCD -= Timer::Instance()->GetDeltaTime();
		if(m_fDeathCD <= 0)
		{
			Destroy();
		}
	}

	if(m_bDead)
		return;

	m_vLastPos = GetWorldPos();
	m_vLastScale = m_vScale;
	m_qLastOrientation = m_qOrientation;

	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->PreLogicUpdate();
	}
	//RefreshTranformMat();
}

void 
GameObject::LogicUpdate()
{
	if(m_bDead)
		return;

	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->LogicUpdate();
	}
	RefreshTranformMat();
}

void 
GameObject::PostLogicUpdate()
{
	if(m_bDead)
		return;

	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->PostLogicUpdate();
	}
	RefreshTranformMat();

	if(m_pCollider != nullptr)
	{
		if(m_vLastPos != GetWorldPos()
			|| m_vLastScale != m_vScale
			|| m_qLastOrientation != m_qOrientation)
		{
			m_bMoved = true;
			if(m_pRigidBody != nullptr)
			{
				if(m_pRigidBody->IsStatic())
				{
					//if it's static object, just wakeup at once
					m_uMovedCount += MIN_MOVED_COUNT * 2;
				}
				else
				{
					++m_uMovedCount;
				}
			}
		}
		else
		{
			m_uMovedCount = 0;
			m_bMoved = false;
		}

		if(m_pRigidBody != nullptr 
			&& m_pRigidBody->IsJustResting() == false 
			&& m_uMovedCount > MIN_MOVED_COUNT)
		{
			m_uMovedCount = 0;
			m_pRigidBody->WakeUpRelyOn();
			m_pRigidBody->LeaveResting();
		}
	}
}

 void GameObject::NetworkUpdate()
 {
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->NetworkUpdate();
	}
 }

bool 
GameObject::IsMoved()
{
	return m_bMoved;
}

void 
GameObject::GraphicUpdate()
{
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->GraphicUpdate();
	}
}

void 
GameObject::RefreshTranformMat()
{
	m_mTranMat = glm::translate(glm::mat4(1.0),m_vPos);
	m_mRotMat = glm::toMat4(m_qOrientation);
	m_mScaleMat =  glm::scale(glm::mat4(1.0),m_vScale);

	m_mLocalWorldMat = m_mTranMat * m_mRotMat * m_mScaleMat;

	if(m_pParent != nullptr)
	{
		m_mWorldMat =  (*GetParentMat()) * m_mLocalWorldMat;	
		m_vWorldPos = Vector3(m_mWorldMat * Vector4(0.0f,0.0f,0.0f,1.0f)); 
	}
	else
	{
		m_mWorldMat = m_mLocalWorldMat;
		m_vWorldPos = m_vPos;
	}

}

Matrix44*
GameObject::GetParentMat()
{
	if(m_pParent != nullptr)
	{
		return m_pParent->GetTranformMat();
	}
	else
	{
		return nullptr;
	}
}

Matrix44*
GameObject::GetTranformMat()
{
	return &m_mWorldMat;
}

Vector3*
GameObject::GetPos()
{
	return &m_vPos;
}

Vector3 
GameObject::GetWorldPos()
{
	if(m_pParent != nullptr)
	{
		return m_vWorldPos;
	}
	else
	{
		return m_vPos;
	}
}

Quaternion
GameObject::GetOrientation()
{
	return m_qOrientation;
}

Vector3 
GameObject::GetScale()
{
	return m_vScale;
}

Vector3 
GameObject::GetRight()
{
	return m_mLocalWorldMat[0].xyz;
}

Vector3 
GameObject::GetUp()
{
	return m_mLocalWorldMat[1].xyz;
}

Vector3 
GameObject::GetForward()
{
	return m_mLocalWorldMat[2].xyz;
}

Vector3 
GameObject::GetWorldRight()
{
	return m_mWorldMat[0].xyz;
}

Vector3 
GameObject::GetWorldUp()
{
	return m_mWorldMat[1].xyz;
}

Vector3 
GameObject::GetWorldForward()
{
	return m_mWorldMat[2].xyz;
}

void 
GameObject::SetPos(Vector3 vPos)
{
	if(m_vPos != vPos)
	{
		if(m_pCollider != nullptr && m_pRigidBody != nullptr)
		{
			m_pRigidBody->LeaveResting();
			m_pRigidBody->WakeUpRelyOn();
		}
	}
	m_vPos = vPos;
}

void 
GameObject::SetRot(Vector3 vRot)
{
	m_qOrientation = glm::quat(glm::vec3(glm::radians(vRot.x),glm::radians(vRot.y),glm::radians(vRot.z)));//should in radian
	SetOrientation(m_qOrientation);
}

void
GameObject::SetRot(Vector3 vAxis, float32 fAngle)
{
	m_qOrientation = glm::angleAxis(fAngle, glm::normalize(vAxis));//should in degree
	SetOrientation(m_qOrientation);
}

void 
GameObject::SetScale(Vector3 vScale)
{
	m_vScale = vScale;

	if(m_pRigidBody != nullptr)
	{
		RefreshTranformMat();
		m_pRigidBody->RefreshMomentInertia();
	}
}

void 
GameObject::SetPos(float32 x, float32 y, float32 z)
{
	if(m_vPos.x != x || m_vPos.y != y || m_vPos.z != z)
	{
		if(m_pCollider != nullptr && m_pRigidBody != nullptr)
		{
			m_pRigidBody->LeaveResting();
			m_pRigidBody->WakeUpRelyOn();
		}
	}
	m_vPos.x = x;
	m_vPos.y = y;
	m_vPos.z = z;
}

void 
GameObject::SetScale(float32 x, float32 y, float32 z)
{
	m_vScale.x = x;
	m_vScale.y = y;
	m_vScale.z = z;

	if(m_pRigidBody != nullptr)
	{
		RefreshTranformMat();
		m_pRigidBody->RefreshMomentInertia();
	}
}

void 
GameObject::SetOrientation(Quaternion& qOrientation)
{
	if(m_qOrientation != qOrientation)
	{
		if(m_pCollider != nullptr && m_pRigidBody!= nullptr)
		{
			m_pRigidBody->LeaveResting();
			m_pRigidBody->WakeUpRelyOn();
		}
	}	

	m_qOrientation = qOrientation;
	m_qOrientation = MFD_Normalize(m_qOrientation);
}

Model*
GameObject::CreateModelComponent(const char* pszMeshName)
{
	assert(m_pModel == nullptr);

	Model* pModel = new Model();

	if(pszMeshName != nullptr)
		pModel->SetMesh(ResourceManager::Instance()->CreateMesh(pszMeshName));

	AddComponent(pModel);
	m_pModel = pModel;

	return pModel;
}

Model* 
GameObject::GetModelComponent()
{
	return m_pModel;
}

Collider* 
GameObject::CreateColliderComponent(const char* pszColliderMeshName)
{
	assert(m_pCollider == nullptr);

	Collider* pCollider = new Collider();

	if(pszColliderMeshName != nullptr)
		pCollider->SetMesh(ResourceManager::Instance()->CreateCollisionMesh(pszColliderMeshName));
	
	AddComponent(pCollider);
	m_pCollider = pCollider;

	return pCollider;

}

Collider* 
GameObject::CreateColliderComponent(float32 fRadius)
{
	assert(m_pCollider == nullptr);

	Collider* pCollider = new Collider();
	pCollider->SetType(ColliderType::Sphere, fRadius);
	
	AddComponent(pCollider);
	m_pCollider = pCollider;

	return pCollider;
}

Collider*	
GameObject::GetColliderComponent()
{
	return m_pCollider;
}

void 
GameObject::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	std::list<GameObjectComponent*>::iterator it;
	for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
	{
		(*it)->OnCollision(pCollidedObj, pCollisionInfo);
	}
}

RigidBody* 
GameObject::CreateRigidBodyComponent()
{
	assert(m_pRigidBody == nullptr);

	RigidBody* pRigidBody = new RigidBody();

	AddComponent(pRigidBody);

	m_pRigidBody = pRigidBody;

	pRigidBody->SetMomentInertiaType(InertiaType::Sphere);//default in case I forget to call

	return pRigidBody;
}

RigidBody*  
GameObject::GetRigidBodyComponent()
{
	return m_pRigidBody;
}

ServerControl*	
GameObject::GetServerControlComponent()
{
	return m_pServerControl;
}

ClientControl*	
GameObject::GetClientControlComponent()
{
	return m_pClientControl;
}

void
GameObject::SetModelComponent(Model* pMode)
{
	Debug::Assert(m_pModel == nullptr);
	AddComponent(pMode);
	m_pModel = pMode;
}

void
GameObject::SetColliderComponent(Collider* pCollider)
{
	Debug::Assert(m_pCollider == nullptr);
	AddComponent(pCollider);
	m_pCollider = pCollider;
}

void
GameObject::SetRigidBodyComponent(RigidBody* pRigidBody)
{
	Debug::Assert(m_pRigidBody == nullptr);
	AddComponent(pRigidBody);
	m_pRigidBody = pRigidBody;
}

void		
GameObject::SetServerControlComponent(ServerControl* pServerControl)
{
	Debug::Assert(m_pServerControl == nullptr);
	AddComponent(pServerControl);
	m_pServerControl = pServerControl;
}

void		
GameObject::SetClientControlComponent(ClientControl* pClientControl)
{
	Debug::Assert(m_pClientControl == nullptr);
	AddComponent(pClientControl);
	m_pClientControl = pClientControl;
}

void 
GameObject::MovePos(Vector3& vMovement)
{
	m_vPos += vMovement;
	SetPos(m_vPos);
}

void 
GameObject::Rotate(Quaternion& qRot)
{
	m_qOrientation = m_qOrientation * qRot;//yes it's *, not +
	SetOrientation(m_qOrientation);
}

void 
GameObject::MoveFB(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}
	

	m_vPos = m_vPos + GetForward() * fValue;
}

void 
GameObject::MoveUD(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}

	m_vPos = m_vPos + GetUp() * fValue;
}

void 
GameObject::MoveLR(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}

	m_vPos = m_vPos + GetRight() * fValue;
}

void 
GameObject::Yaw(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}

	Matrix44 mRot = glm::rotate(fValue,GetUp());
	Quaternion qRot = glm::toQuat(mRot);
	Rotate(qRot);

	RefreshTranformMat();
}

void 
GameObject::Roll(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}

	Matrix44 mRot = glm::rotate(fValue,GetForward());
	Quaternion qRot = glm::toQuat(mRot);
	Rotate(qRot);

	RefreshTranformMat();
}

void 
GameObject::Pitch(float32 fValue)
{
	if(fValue != 0 && m_pCollider != nullptr && m_pRigidBody != nullptr)
	{
		m_pRigidBody->LeaveResting();
		m_pRigidBody->WakeUpRelyOn();
	}

	Matrix44 mRot = glm::rotate(fValue,GetRight());
	Quaternion qRot = glm::toQuat(mRot);
	Rotate(qRot);

	RefreshTranformMat();
}

void 
GameObject::Destroy(float32 fDelay)
{
	if(fDelay > 0)
	{
		m_fDeathCD = fDelay;
	}
	else
	{
		m_bDead = true;
		std::list<GameObjectComponent*>::iterator it;
		for(it = m_componentList.begin(); it != m_componentList.end(); ++it)
		{
			(*it)->Enable(false);
		}
	}
}

bool 
GameObject::IsDead()
{
	return m_bDead || m_fDeathCD > 0;
}