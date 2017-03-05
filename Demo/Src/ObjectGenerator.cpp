#include "Itsukushima_include.h"
#include "ObjectGenerator.h"

ObjectGenerator::ObjectGenerator(void)
{
	m_pszObjectName = nullptr;
	m_pszModelName = nullptr;
	m_pszMaterialName = nullptr;
	m_pszColliderName = nullptr;
	m_eInertiaType = InertiaType::Box;

	m_pGeneratedObject = nullptr;

	m_vRotation = Vector3(0.0f);

	m_fDelay = 10.0f;
	m_fCD = 0.0f;
}


ObjectGenerator::~ObjectGenerator(void)
{
	m_pszObjectName = nullptr;
	m_pszModelName = nullptr;
	m_pszMaterialName = nullptr;
	m_pszColliderName = nullptr;
	m_pGeneratedObject = nullptr;
}


const char* 
ObjectGenerator::ClassName()
{
	return "ObjectGenerator";
}

const char* 
ObjectGenerator::GetName()
{
	return "ObjectGenerator";
}

void 
ObjectGenerator::PostLogicUpdate()
{
	if(m_pGeneratedObject == nullptr)
	{
		m_fCD -= Timer::Instance()->GetDeltaTime();
		if(m_fCD < 0)
		{
			GameObject* pObject = nullptr;
			Model* pModel = nullptr;
			RigidBody* pRigidBody = nullptr;

			pObject = m_pGameObject->GetScene()->CreateGameObject(m_pszObjectName);
			pObject->SetPos(m_pGameObject->GetWorldPos());
			pObject->SetRot(m_vRotation);
			pObject->SetScale(m_vScale);
			pModel = pObject->CreateModelComponent(m_pszModelName);
			pModel->SetMaterial(m_pszMaterialName);
			pObject->CreateColliderComponent(m_pszColliderName);
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetMomentInertiaType(m_eInertiaType);
			m_pGeneratedObject = pObject;
		}
	}
	else if(m_pGeneratedObject->IsDead())
	{
		m_fCD = m_fDelay;
		m_pGeneratedObject = nullptr;
	}
	else if(m_pGeneratedObject->GetWorldPos().y < -10)
	{
		m_pGeneratedObject->Destroy();
		m_pGeneratedObject = nullptr;
	}
}

void 
ObjectGenerator::SetObjectName(const char* pszName)
{
	m_pszObjectName = pszName;
}

void 
ObjectGenerator::SetModelName(const char* pszName)
{
	m_pszModelName = pszName;
}

void 
ObjectGenerator::SetMaterialName(const char* pszName)
{
	m_pszMaterialName = pszName;
}

void
ObjectGenerator::SetColliderName(const char* pszName)
{
	m_pszColliderName = pszName;
}

void 
ObjectGenerator::SetInertiaType(InertiaType eType)
{
	m_eInertiaType = eType;
}

void 
ObjectGenerator::SetRotation(Vector3 vRotation)
{
	m_vRotation = vRotation;
}

void 
ObjectGenerator::SeScale(Vector3 vScale)
{
	m_vScale = vScale;
}

void 
ObjectGenerator::SetDelay(float32 fDelay)
{
	m_fDelay = fDelay;
}