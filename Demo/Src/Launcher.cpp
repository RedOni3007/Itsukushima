#include "Itsukushima_include.h"
#include "Launcher.h"
#include "ObjectPool.h"


Launcher::Launcher()
{
	m_pObjectPool = nullptr;
	m_fInitCD = 1.0f;
	m_fCD = m_fInitCD;

	m_fMass = 1.0f;
	m_fDynamicFriction = 0.1f;
	m_fStaticFriction = 0.4f;
	m_fElasticity = 0.0f;
}


Launcher::~Launcher()
{
	m_pObjectPool = nullptr;
}

void 
Launcher::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	m_fCD-= fDeltaTime;
	if(m_fCD < 0)
	{
		m_fCD = m_fInitCD;
		LaunchOjbect();
	}
}


const char*
Launcher::GetName()
{
	return "Launcher";
}

const char* 
Launcher::ClassName()
{
	return "Launcher";
}

void
Launcher::SetObjectPool(ObjectPool* pObjectPool)
{
	m_pObjectPool = pObjectPool;
}

void 
Launcher::LaunchOjbect()
{
	GameObject* pObject = m_pObjectPool->GetNextObject();
	RigidBody* pRigidBody = pObject->GetRigidBodyComponent();
	pRigidBody->LeaveResting();
	pRigidBody->ClearForce();

	pObject->SetPos(m_pGameObject->GetWorldPos());
	pObject->SetRot(Vector3(MathHelper::RandI(0,360),MathHelper::RandI(0,360),MathHelper::RandI(0,360)));

	pRigidBody->SetVelocity(m_vStartVelocity);
	pRigidBody->SetMass(m_fMass);
	pRigidBody->SetDynamicFriction(m_fDynamicFriction);
	pRigidBody->SetStaticFriction(m_fStaticFriction);
	pRigidBody->SetElasticity(m_fElasticity);
}

void 
Launcher::SetStartVelocity(Vector3 vValue)
{
	m_vStartVelocity = vValue;
}

void 
Launcher::SetStartRotation(Vector3 vValue)
{
	m_vStartRot = vValue;
}

void 
Launcher::SetMass(float32 fValue)
{
	m_fMass = fValue;
}

void 
Launcher::SetDynamicFriction(float32 fValue)
{
	m_fDynamicFriction = fValue;
}

void 
Launcher::SetStaticFriction(float32 fValue)
{
	m_fStaticFriction = fValue;
}

void 
Launcher::SetElasticity(float32 fValue)
{
	m_fElasticity = fValue;
}


Vector3 
Launcher::GetStartVelocity()
{
	return m_vStartVelocity;
}

Vector3 
Launcher::GetStartRotation()
{
	return m_vStartRot;
}

float32 
Launcher::GetMass()
{
	return m_fMass;
}

float32 
Launcher::GetDynamicFriction()
{
	return m_fDynamicFriction;
}

float32 
Launcher::GetStaticFriction()
{
	return m_fStaticFriction;
}

float32
Launcher::GetElasticity()
{
	return m_fElasticity;
}
