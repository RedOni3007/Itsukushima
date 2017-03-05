#include "Itsukushima_include.h"
#include "ObjectPool.h"

ObjectPool::ObjectPool()
{
	m_pScene = nullptr;
	m_uMaxNumber = 0;
	m_uCurrentIndex = 0;
}


ObjectPool::~ObjectPool()
{
}


void 
ObjectPool::Init(Scene* pScene, uint32 uMaxNumber)
{
	m_pScene = pScene;
	m_uMaxNumber = uMaxNumber;
	m_objects.reserve(uMaxNumber);
}

GameObject* 
ObjectPool::GetNextObject()
{
	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	RigidBody* pRigidBody = nullptr;
	if(m_objects.size() < m_uMaxNumber)
	{
		//crate a object
		char nameBuffer [NAME_CBUFFER_SIZE];

		//crate a object
		sprintf_s(nameBuffer, NAME_CBUFFER_SIZE,"PoolObject_%d", (uint32)m_objects.size());
		pObject = m_pScene->CreateGameObject(nameBuffer);

		//not really matter if the chances are even
		if(rand()%1000 < 200)
		{
			pModel = pObject->CreateModelComponent("CUBE_LOW");
			pModel->SetMaterial("Box");
			pObject->CreateColliderComponent("Box_Collision");
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(InertiaType::Box);			
		}
		else if(rand()%1000 < 250)
		{
			pModel = pObject->CreateModelComponent("CYLINDER");
			pModel->SetMaterial("Cylinder");
			pObject->CreateColliderComponent("Cylinder_Collision");
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(InertiaType::Cylinder);			
		}
		else if(rand()%1000 < 333)
		{
			pModel = pObject->CreateModelComponent("STRANGE");
			pModel->SetMaterial("Strange");
			pObject->CreateColliderComponent("Strange_Collision");
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(InertiaType::Box);
		}
		else if(rand()%1000 < 500)
		{
			pModel = pObject->CreateModelComponent("SPHERE");
			pModel->SetMaterial("Sphere");
			pObject->CreateColliderComponent(0.5f);
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(InertiaType::Sphere);
		}
		else
		{
			pModel = pObject->CreateModelComponent("CROSS");
			pModel->SetMaterial("Cross");
			pObject->CreateColliderComponent("Cross_Collision");
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(InertiaType::Box);		
		}

		m_objects.push_back(pObject);
	}
	else 
	{
		pObject = m_objects[m_uCurrentIndex];
		++m_uCurrentIndex;

		if(m_uCurrentIndex >= m_uMaxNumber)
			m_uCurrentIndex = 0;
	}

	return pObject;

}