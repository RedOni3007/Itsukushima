#include <Physics/RigidBody.h>
#include "PhysicsConstants.h"
#include <App/Timer.h>
#include <Physics/Collider.h>
#include <Physics/CollisionManager.h>
#include <Math/MathHelper.h>

#define STOP_ON_TOUCH 0

#define MIN_MASS 0.2f

#define MIN_RESTING_VELOCITY_LENGTH 0.1f
#define IGNORED_VELOCITY_LENGTH 0.1f //need to tweak

#define LIMIT_SINGLE_TORQUE 0
#define LIMIT_TOTAL_TORQUE 0
#define MAX_SINGLE_TORQUE_LENGTH 1000.0f //need to tweak
#define MAX_TOTAL_TORQUE_LENGTH 3000.0f  //need to tweak

#define ROTATION_STABILIZE_THRESHOLD 2.0f

#define TINY_SHAKE_BALANCE 0

#define RELY_ON_LIST_SIZE 256

RigidBody::RigidBody(void)
{
	m_bStatic = false;
	m_bTriggerable = false;

	m_fMass = 1.0f;
	m_fMassInversed = 1.0f / m_fMass;
	m_vCenterOfMass_local = Vector3(0.0f,0.0f,0.0f);
	m_vCenterOfMass_world =  Vector3(0.0f,0.0f,0.0f);
	m_vForce = Vector3(0.0f,0.0f,0.0f);
	m_vVelocity = Vector3(0.0f,0.0f,0.0f);
	m_vLastVelocity = Vector3(0.0f,0.0f,0.0f);
	m_vAngularVelocity = Vector3(0.0f,0.0f,0.0f);
	m_vLastAngularVelocity = Vector3(0.0f,0.0f,0.0f);
	m_vVelocityChange =  Vector3(0.0f,0.0f,0.0f);
	m_vAngularVelocityChange = Vector3(0.0f,0.0f,0.0f);

	m_vTorque = Vector3(0.0f,0.0f,0.0f);
	m_vConstantForce = Vector3(0.0f,0.0f,0.0f);

	m_fElasticity = 0.0f;

	m_fResistance = 0.01f;
	m_fAngularResistance = 0.05f;

	m_fStaticFriction = 0.5f;
	m_fDynamicFriction = 0.1f;

	m_mMomentInertia = Matrix33(1.0f);

	m_bResting = false;

	m_mMomentInertiaWorld = Matrix33(1.0f);

	m_bGravity = true;

	m_eInertiaType = InertiaType::Box;

	m_bTouched = false;

	m_uRestingCount = 0;

	m_RelyOnBodies.reserve(RELY_ON_LIST_SIZE);
	for(uint32 i = 0; i < RELY_ON_LIST_SIZE; ++i)
	{
		m_RelyOnBodies.push_back(nullptr);
	}
	m_uRelyOnListSize = 0;
}


RigidBody::~RigidBody(void)
{
}

RigidBody* 
RigidBody::CreateCharacteristicClone()
{
	RigidBody* pClone = new RigidBody();
	pClone->m_bStatic = m_bStatic;
	pClone->m_bTriggerable = m_bTriggerable;
	pClone->m_bGravity = m_bGravity;
	pClone->m_fMass = m_fMass;
	pClone->m_fMassInversed = m_fMassInversed;
	pClone->m_vCenterOfMass_local = m_vCenterOfMass_local;
	pClone->m_vVelocity = m_vVelocity;
	pClone->m_vLastVelocity = m_vLastVelocity;
	pClone->m_vAngularVelocity = m_vAngularVelocity;
	pClone->m_vLastAngularVelocity = m_vLastAngularVelocity;
	pClone->m_vConstantForce = m_vConstantForce;
	pClone->m_fElasticity = m_fElasticity;
	pClone->m_fResistance = m_fResistance;
	pClone->m_fAngularResistance = m_fAngularResistance;
	pClone->m_fStaticFriction = m_fStaticFriction;
	pClone->m_fDynamicFriction = m_fDynamicFriction;
	pClone->m_eInertiaType = m_eInertiaType;

	return pClone;
}


bool 
RigidBody::IsStatic()
{
	return m_bStatic;
}

void 
RigidBody::SetMomentInertiaType(InertiaType eType)
{
	m_eInertiaType = eType;
	RefreshMomentInertia();
}

void
RigidBody::RefreshMomentInertia()
{
	if(m_eInertiaType == InertiaType::Box)
	{
		UseBoxMomentInertia();
	}
	else if(m_eInertiaType == InertiaType::Cylinder)
	{
		UseCylinderMomentInertia();
	}
	else if(m_eInertiaType == InertiaType::D4)
	{
		UseD4MomentInertia();
	}
	else if(m_eInertiaType == InertiaType::Ellipsoid)
	{
		UseEllipsoidMomentInertia();
	}
	else if(m_eInertiaType == InertiaType::Sphere)
	{
		UseSphereMomentInertia();
	}
	else if(m_eInertiaType == InertiaType::Cone)
	{
		UseConeMomentInertia();
	}
	else if(m_eInertiaType == InertiaType::Custom)
	{
		assert(!"too much suger in my tea");
	}
}

void 
RigidBody::UseSphereMomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.4f * m_fMass * pCollider->GetRange() * pCollider->GetRange();
	m_mMomentInertia[1][1] = m_mMomentInertia[0][0];
	m_mMomentInertia[2][2] = m_mMomentInertia[0][0];

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertiaInverse);
}

void 
RigidBody::UseBoxMomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	if(pCollider->GetType() == ColliderType::Sphere)
	{
		assert(!"Wrong type");
		return;
	}

	CollisionMesh *pColliderMesh = pCollider->GetCollisionMesh();
	Vector3 vScale = m_pGameObject->GetScale();

	float32 fLengthX2 = pColliderMesh->GetLengthX() * vScale.x;
	fLengthX2 *= fLengthX2;
	float32 fLengthY2 = pColliderMesh->GetLengthY() * vScale.y;
	fLengthY2 *= fLengthY2;
	float32 fLengthZ2 = pColliderMesh->GetLengthZ() * vScale.z;
	fLengthZ2 *= fLengthZ2;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.083f * m_fMass * (fLengthY2 + fLengthZ2);
	m_mMomentInertia[1][1] = 0.083f * m_fMass * (fLengthX2 + fLengthZ2);
	m_mMomentInertia[2][2] = 0.083f * m_fMass * (fLengthX2 + fLengthY2);

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertia);
}

void 
RigidBody::UseCylinderMomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	if(pCollider->GetType() == ColliderType::Sphere)
	{
		assert(!"Wrong type");
		return;
	}

	CollisionMesh *pColliderMesh = pCollider->GetCollisionMesh();
	Vector3 vScale = m_pGameObject->GetScale();

	//+y axis is up
	float32 fR2 = pColliderMesh->GetLengthX() * vScale.x * 0.5f;
	fR2 *= fR2;
	float32 fH2= pColliderMesh->GetLengthY() * vScale.y;
	fH2 *= fH2;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.083f * m_fMass * (3 * fR2 + fH2);
	m_mMomentInertia[1][1] = 0.5f * m_fMass * (fR2);
	m_mMomentInertia[2][2] = 0.083f * m_fMass * (3 * fR2 + fH2);

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertia);
}
	
//not really correct
void 
RigidBody::UseD4MomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	if(pCollider->GetType() == ColliderType::Sphere)
	{
		assert(!"Wrong type");
		return;
	}

	CollisionMesh *pColliderMesh = pCollider->GetCollisionMesh();
	Vector3 vScale = m_pGameObject->GetScale();

	//+y axis is up
	float32 fLengthX2 = pColliderMesh->GetLengthX() * vScale.x * 0.5f;
	fLengthX2 *= fLengthX2;
	float32 fLengthY2 = pColliderMesh->GetLengthY() * vScale.y;
	fLengthY2 *= fLengthY2;
	float32 fLengthZ2 = pColliderMesh->GetLengthZ() * vScale.z * 0.5f;
	fLengthZ2 *= fLengthZ2;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.2f * m_fMass * fLengthZ2 + 0.0375f * m_fMass * fLengthY2;
	m_mMomentInertia[1][1] = 0.2f * m_fMass * (fLengthX2 + fLengthZ2);
	m_mMomentInertia[2][2] = 0.2f * m_fMass * fLengthX2 + 0.0375f * m_fMass * fLengthY2;

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertia);
}
	
void 
RigidBody::UseEllipsoidMomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	if(pCollider->GetType() == ColliderType::Sphere)
	{
		assert(!"Wrong type");
		return;
	}

	CollisionMesh *pColliderMesh = pCollider->GetCollisionMesh();
	Vector3 vScale = m_pGameObject->GetScale();

	//+y axis is up
	float32 fLengthX2 = pColliderMesh->GetLengthX() * vScale.x * 0.5f;
	fLengthX2 *= fLengthX2;
	float32 fLengthY2 = pColliderMesh->GetLengthY() * vScale.y * 0.5f;
	fLengthY2 *= fLengthY2;
	float32 fLengthZ2 = pColliderMesh->GetLengthZ() * vScale.z * 0.5f;
	fLengthZ2 *= fLengthZ2;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.2f * m_fMass * (fLengthY2 + fLengthZ2);
	m_mMomentInertia[1][1] = 0.2f * m_fMass * (fLengthX2 + fLengthZ2);
	m_mMomentInertia[2][2] = 0.2f * m_fMass * (fLengthX2 + fLengthY2);

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertia);
}

void 
RigidBody::UseConeMomentInertia()
{
	Collider *pCollider = m_pGameObject->GetColliderComponent();
	if(pCollider == nullptr)
		return;

	if(pCollider->GetType() == ColliderType::Sphere)
	{
		assert(!"Wrong type");
		return;
	}

	CollisionMesh *pColliderMesh = pCollider->GetCollisionMesh();
	Vector3 vScale = m_pGameObject->GetScale();

	//+y axis is up
	float32 fLengthX2 = pColliderMesh->GetLengthX() * vScale.x * 0.5f;
	fLengthX2 *= fLengthX2;
	float32 fLengthY2 = pColliderMesh->GetLengthY() * vScale.y * 0.5f;
	fLengthY2 *= fLengthY2;
	float32 fLengthZ2 = pColliderMesh->GetLengthZ() * vScale.z * 0.5f;
	fLengthZ2 *= fLengthZ2;

	m_mMomentInertia = Matrix33(0.0f);//clear
	m_mMomentInertia[0][0] = 0.6f * m_fMass * fLengthY2 + 0.15f * m_fMass * fLengthX2;
	m_mMomentInertia[1][1] = 0.6f * m_fMass * fLengthY2 + 0.15f * m_fMass * fLengthX2;
	m_mMomentInertia[2][2] = 0.3f * m_fMass * fLengthX2;

	m_mMomentInertiaInverse = MFD_Inverse(m_mMomentInertia);

}

void 
RigidBody::EnterResting()
{
	if(m_bStatic)
		return;

	m_bResting = true;
}

void 
RigidBody::LeaveResting()
{
	if(m_bStatic)
		return;

	m_bResting = false;
	m_uRestingCount = 0;	
}

bool 
RigidBody::IsJustResting()
{
	return m_bResting && m_uRestingCount <= 1;
}

void 
RigidBody::WakeUpRelyOn()
{
	for(uint32 i = 0; i < m_uRelyOnListSize; ++i)
	{
		m_RelyOnBodies[i]->LeaveResting();
	}
	m_uRelyOnListSize = 0;
}

void 
RigidBody::AddToRelyOnList(RigidBody* pBody)
{
	for(uint32 i = 0; i < m_uRelyOnListSize; ++i)
	{
		if(m_RelyOnBodies[i] == pBody)
			return;
	}

	assert(m_uRelyOnListSize < RELY_ON_LIST_SIZE);

	m_RelyOnBodies[m_uRelyOnListSize] = pBody;
	++m_uRelyOnListSize;
}

void 
RigidBody::ClearForce()
{
	m_vForce.x = m_vForce.y = m_vForce.z = 0;
	m_vTorque.x = m_vTorque.y = m_vTorque.z = 0;
}

void 
RigidBody::PreLogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	if(m_bResting)
	{
		if(IsJustResting())
		{
			m_vForce.x = m_vForce.y = m_vForce.z = 0;
			m_vTorque.x = m_vTorque.y = m_vTorque.z = 0;
		}

		++m_uRestingCount;

		m_vVelocity = m_vVelocity * 0.0f;

#if TINY_SHAKE_BALANCE
		if(m_bTouched)
		{
			m_vAngularVelocity = m_vAngularVelocity * -0.9f;//shake a little bit for better fit
		}

		if(MathHelper::IsZeroVector(m_vAngularVelocity)==false)
		{
			m_vVelocity +=  PhysicsContants::Gravity * fDeltaTime * 0.1f;	
		}
#else
		m_vAngularVelocity = m_vAngularVelocity * 0.0f;
#endif

		MathHelper::Clean(m_vVelocity);
		MathHelper::Clean(m_vAngularVelocity);
	}
	else
	{
		//stabilize a little bit to prevent micro shaking
		if(m_bStatic == false && m_eInertiaType != InertiaType::Sphere &&MFD_Length(m_vAngularVelocity) < ROTATION_STABILIZE_THRESHOLD)
		{
			m_vAngularVelocity = m_vAngularVelocity * 0.95f;
		}
	}

#if !STOP_ON_TOUCH
	m_bTouched = false;
#endif

	m_bNoResting = false;
}

void 
RigidBody::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	if(m_bActive == false)
	{
		return;
	}

#if STOP_ON_TOUCH
	if(m_bTouched)
	{
		return;
	}
#endif

	m_vLastVelocity = m_vVelocity;
	m_vLastAngularVelocity = m_vAngularVelocity;

	//apply the velocity changs
	m_vVelocity += m_vVelocityChange;
	m_vAngularVelocity += m_vAngularVelocityChange;

	m_vVelocityChange.x = m_vVelocityChange.y = m_vVelocityChange.z = 0;
	m_vAngularVelocityChange.x = m_vAngularVelocityChange.y = m_vAngularVelocityChange.z = 0;

	//update pos
	if(m_bGravity && m_bResting == false && m_bStatic == false)
	{
		m_vForce +=  PhysicsContants::Gravity * m_fMass;		
	}

	Vector3 vAcceleration = (m_vForce + m_vConstantForce) * m_fMassInversed;
	m_vVelocity += vAcceleration * fDeltaTime;
	Vector3 vMovement = m_vVelocity * fDeltaTime + vAcceleration * fDeltaTime * fDeltaTime * 0.5f;
	m_pGameObject->MovePos(vMovement);

	//update rotation
	m_vAngularVelocity += m_vTorque * m_mMomentInertiaWorld * fDeltaTime;
	Quaternion qOriginRotation = m_pGameObject->GetOrientation();
	Quaternion qRotation = (Quaternion(0, m_vAngularVelocity.x,  m_vAngularVelocity.y,  m_vAngularVelocity.z) * qOriginRotation) * 0.5f * fDeltaTime;
	qRotation = qOriginRotation + qRotation;
	m_pGameObject->SetOrientation(qRotation);

	Matrix44 &mWorldMat = *m_pGameObject->GetTranformMat();
	m_vCenterOfMass_world =  glm::vec3(mWorldMat * Vector4(m_vCenterOfMass_local,1.0f));

	qRotation = m_pGameObject->GetOrientation();
	Matrix33 mRotMat = glm::toMat3(qRotation);
	Matrix33 mRotMatTranpose = glm::transpose(mRotMat);

	m_mMomentInertiaWorld = mRotMat * m_mMomentInertiaInverse * mRotMatTranpose;
	m_mMomentInertiaWorldInverse = MFD_Inverse(m_mMomentInertiaWorld);

	if(m_bStatic == false)
	{
		m_vVelocity *= (1 - m_fResistance * fDeltaTime);
		m_vAngularVelocity *= (1 - m_fAngularResistance * fDeltaTime);
	}
	//clear force in this frame
	m_vForce.x = m_vForce.y = m_vForce.z = 0;
	m_vTorque.x = m_vTorque.y = m_vTorque.z = 0;

	CleanTinyNumbers();
}

void 
RigidBody::PostLogicUpdate()
{

}

void 
RigidBody::CleanTinyNumbers()
{
	if(fabsf(m_vVelocity.x) < 0.00001f)
		m_vVelocity.x = 0;

	if(fabsf(m_vVelocity.y) < 0.00001f)
		m_vVelocity.y = 0;

	if(fabsf(m_vVelocity.z) < 0.00001f)
		m_vVelocity.z = 0;

	if(fabsf(m_vAngularVelocity.x) < 0.00001f)
		m_vAngularVelocity.x = 0;

	if(fabsf(m_vAngularVelocity.y) < 0.00001f)
		m_vAngularVelocity.y = 0;

	if(fabsf(m_vAngularVelocity.z) < 0.00001f)
		m_vAngularVelocity.z = 0;

	//MathHelper::Clean(m_vVelocity);
	//MathHelper::Clean(m_vAngularVelocity);
}

void 
RigidBody::GraphicUpdate()
{
	if(m_bStatic || m_bActive == false)
	{
		return;
	}


}

const char* 
RigidBody::GetName()
{
	return "RigidBody";
}

const char* 
RigidBody::ClassName()
{
	return "RigidBody";
}

void 
RigidBody::SetStatic(bool bStatic)
{
	m_bStatic = bStatic;

	if(m_bStatic)
	{
		m_vForce = Vector3(0);
		m_vTorque = Vector3(0);
	}
}

void 
RigidBody::SetTriggerable(bool bTriggerable)
{
	m_bTriggerable = bTriggerable;

	Enable(!m_bTriggerable);
	
}

float32 
RigidBody::GetMass()
{
	return m_fMass;
}

void 
RigidBody::SetMass(float32 fMass)
{
	assert(fMass > 0);

	fMass = MFD_Max(MIN_MASS,fMass);

	m_fMass = fMass;
	m_fMassInversed = 1.0f / m_fMass;
	RefreshMomentInertia();
}

void 
RigidBody::SetElasticity(float32 fElasticity)
{
	m_fElasticity = fElasticity;

	m_fElasticity = MFD_Min(m_fElasticity,1.0f);//maybe should allow it >1, it's more interesting
	m_fElasticity = MFD_Max(m_fElasticity,0.0f);
}

void 
RigidBody::SetStaticFriction(float32 fVal)
{
	m_fStaticFriction = fVal;
}

void 
RigidBody::SetDynamicFriction(float32 fVal)
{
	m_fDynamicFriction = fVal;
}

void 
RigidBody::SetVelocity(Vector3 vVel)
{
	m_vVelocity = vVel;
}

void 
RigidBody::SetAngularVelocity(Vector3 vVel)
{
	m_vAngularVelocity = vVel;
}

void 
RigidBody::AddForce(Vector3 vPos, Vector3 vForce)
{
	if(m_bTriggerable && m_bActive == false)
	{
		Enable(true);
	}

	if(m_bStatic || m_bActive == false)
	{
		return;
	}

	if(IsJustResting())
	{
		return;
	}

	if(vForce.x == 0 && vForce.y == 0 && vForce.z == 0)
		return;

	m_pGameObject->RefreshTranformMat();
	Matrix44 &mWorldMat = *m_pGameObject->GetTranformMat();
	m_vCenterOfMass_world =  glm::vec3(mWorldMat * Vector4(m_vCenterOfMass_local,1.0f));

	Vector3 vR = vPos - m_vCenterOfMass_world;
	float32 fRDotF = MFD_Dot(MFD_Normalize(vR), MFD_Normalize(vForce));  

	if(fRDotF < 0)
	{
		fRDotF = -fRDotF;
	}

	Vector3 vMovementForce = vForce * fRDotF;

	AddMovementForce(vMovementForce);

	Vector3 vTorque = MFD_Cross(vR, vForce);// * (1 - fRDotF);//not sure if need this "* (1 - fRDotF)" part
	AddTorque(vTorque);
}

void 
RigidBody::AddRotationForce(Vector3 vPos, Vector3 vForce)
{
	if(m_bTriggerable && m_bActive == false)
	{
		Enable(true);
	}

	if(m_bStatic || m_bActive == false)
	{
		return;
	}

	if(IsJustResting())
	{
		return;
	}

	m_pGameObject->RefreshTranformMat();
	Matrix44 &mWorldMat = *m_pGameObject->GetTranformMat();
	m_vCenterOfMass_world =  glm::vec3(mWorldMat * Vector4(m_vCenterOfMass_local,1.0f));

	Vector3 vR = vPos - m_vCenterOfMass_world;
	float32 fRDotF = MFD_Dot(MFD_Normalize(vR), MFD_Normalize(vForce));  

	if(fRDotF < 0)
	{
		fRDotF = -fRDotF;
	}

	Vector3 vTorque = MFD_Cross(vR, vForce);// * (1 - fRDotF);//not sure if need this "* (1 - fRDotF)" part
	AddTorque(vTorque);
}

void 
RigidBody::AddMovementForce(Vector3 vForce)
{
	if(m_bTriggerable && m_bActive == false)
	{
		Enable(true);
	}

	if(m_bStatic || m_bActive == false)
	{
		return;
	}

	if(IsJustResting())
	{
		return;
	}

	m_vForce += vForce;

	LeaveResting();

}

void RigidBody::AddTorque(Vector3 vTorque)
{
	if(m_bTriggerable && m_bActive == false)
	{
		Enable(true);
	}

	if(m_bStatic || m_bActive == false)
	{
		return;
	}

	if(IsJustResting())
	{
		return;
	}

#if LIMIT_SINGLE_TORQUE
	if( MFD_Length(vTorque) > MAX_SINGLE_TORQUE_LENGTH)
	{
		vTorque = MFD_Normalize(vTorque) * MAX_SINGLE_TORQUE_LENGTH;
	}
	m_vTorque += vTorque;
#else
	m_vTorque += vTorque;
#endif

#if LIMIT_TOTAL_TORQUE
	if(MFD_Length(m_vTorque) > MAX_TOTAL_TORQUE_LENGTH)
	{
		m_vTorque = MFD_Normalize(m_vTorque) * MAX_TOTAL_TORQUE_LENGTH;
	}
#endif

	LeaveResting();
}

void 
RigidBody::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	m_bTouched = true;
	
	if(m_bTriggerable && m_bActive == false && pCollidedObj->GetRigidBodyComponent() != nullptr)
	{
		Enable(true);
	}
}