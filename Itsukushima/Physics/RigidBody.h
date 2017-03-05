/**
* Rigid Body, the collision is handled in CollisionManager
* todo: implement create custom inertia tensor from mesh
*
* @author: Kai Yang
**/

#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include <Game/GameObjectComponent.h>
#include <Core/CoreHeaders.h>

class CollisionManager;

enum class InertiaType : int32
{
	Sphere = 0,
	Cylinder,
	Ellipsoid,
	D4,
	Box,
	Cone,
	Custom,//not in use
};

class RigidBody : public GameObjectComponent
{
friend class CollisionManager;

public:
	RigidBody();
	virtual ~RigidBody();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	static const char* ClassName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

	float32 GetMass();

	void SetMass(float32 fMass);
	void SetElasticity(float32 fElasticity);
	void SetStaticFriction(float32 fVal);
	void SetDynamicFriction(float32 fVal);

	void SetStatic(bool bStatic);
	void SetTriggerable(bool bTriggerable);

	void SetMomentInertiaType(InertiaType eType);

	void SetVelocity(Vector3 vVel);
	void SetAngularVelocity(Vector3 vVel);

	void AddForce(Vector3 vPos, Vector3 vForce);
	void AddMovementForce(Vector3 vForce);//on centre of mass
	void AddRotationForce(Vector3 vPos, Vector3 vForce);

	void AddTorque(Vector3 vTorque);

	void EnterResting();
	void LeaveResting();
	bool IsJustResting();

	void AddToRelyOnList(RigidBody* pBody);
	void WakeUpRelyOn();

	void ClearForce();

	void RefreshMomentInertia();

	bool IsStatic();

	//need to call RefreshMomentInertia later
	RigidBody* CreateCharacteristicClone();

private:
	void CleanTinyNumbers();

	void UseSphereMomentInertia();
	void UseBoxMomentInertia();
	void UseCylinderMomentInertia();
	void UseD4MomentInertia();//Can't spell Tetrahedron correct every time, so it's a d4 dice, and the I matrix is not really correct
	void UseEllipsoidMomentInertia();
	void UseConeMomentInertia();

private:
	bool m_bResting;
	bool m_bNoResting;//current frame

	bool m_bStatic;//will not affect by external force, but still can have velocity
	bool m_bTriggerable;

	bool m_bGravity;

	float32 m_fMass;
	float32 m_fMassInversed;
	Vector3 m_vCenterOfMass_local;//in local space
	Vector3 m_vCenterOfMass_world;//in world space

	Vector3 m_vVelocity;
	Vector3 m_vLastVelocity;
	Vector3 m_vAngularVelocity;
	Vector3 m_vLastAngularVelocity;

	Vector3 m_vVelocityChange;
	Vector3 m_vAngularVelocityChange;

	Vector3 m_vConstantForce;//for now: gravity only
	Vector3 m_vForce;//total force every frame

	float32 m_fElasticity;

	float32 m_fResistance;//percentage per second
	float32 m_fAngularResistance;//percentage per second

	Vector3  m_vTorque;
	Matrix33 m_mMomentInertia;
	Matrix33 m_mMomentInertiaInverse;
	Matrix33 m_mMomentInertiaWorld;
	Matrix33 m_mMomentInertiaWorldInverse;

	float32 m_fStaticFriction;
	float32 m_fDynamicFriction;

	InertiaType m_eInertiaType;

	std::vector<RigidBody*> m_RelyOnBodies;
	uint32 m_uRelyOnListSize;

	bool m_bTouched;
	uint32 m_uRestingCount;
};

#endif