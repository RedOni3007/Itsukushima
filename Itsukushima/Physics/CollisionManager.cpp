#include <Physics/CollisionManager.h>
#include <Math/CollisionHelper.h>
#include <Resource/Mesh.h>
#include <Resource/Model.h>
#include <Resource/ResourceManager.h>
#include <Math/MathHelper.h>
#include <Game/GameObject.h>
#include <Physics/Collider.h>
#include <Game/Scene.h>
#include <Game/GameObjectComponent.h>
#include <Physics/RigidBody.h>
#include <App/Timer.h>
#include "PhysicsConstants.h"

#include <list>
#include <time.h>
#include <thread>

#define DISABLE_COLLISION 0

#define SEPARATE_BY_NORMAL 1
#define TOTAL_SETP_SEPARATE 20 //tweakable
#define OVERLAP_RANGE_SEPARATE 2.0f //tweakable
#define MIN_SETP_LENGTH_SEPARATE 0.005f //tweakable

#define CONVERT_IMPULSE_TO_FORCE 1

#define USE_FRAME_SUM_VELOCITY 1
#define COLLISION_ENERGY_LOSS 0.00f


CollisionManager::CollisionManager(void)
{
}


CollisionManager::~CollisionManager(void)
{
}

CollisionManager* 
CollisionManager::Instance()
{
	static CollisionManager me;
	return &me;
}

void 
CollisionManager::PrepareCollisionInfo(CollisionInfo *pCollisionInfo)
{
	MathHelper::RemoveDuplicate(pCollisionInfo->touchedPostions,pCollisionInfo->uTouchPostionCount);

	RigidBody *pBody1 = pCollisionInfo->pObject1->GetRigidBodyComponent();
	RigidBody *pBody2 = pCollisionInfo->pObject2->GetRigidBodyComponent();

	if(pBody1 == nullptr || pBody2 == nullptr)
		return;

	Collider *pCollider1 = pCollisionInfo->pObject1->GetColliderComponent();
	Collider *pCollider2 = pCollisionInfo->pObject2->GetColliderComponent();

	//calculate normal, suppose two object only will have one touched point(or area)
	Vector3 vTouchedPoint(0.0f);
	Vector3 vNormal(0.0f);
	Face *face = nullptr;
	if(pCollider1->GetType() == ColliderType::Sphere && pCollider2->GetType() == ColliderType::Mesh)
	{
		//object1 should be the sphere one
		std::vector<Face> *pFaces = pCollisionInfo->pObject2->GetColliderComponent()->GetCollisionMesh()->GetFaces();
		for(uint32 i = 0; i < pCollisionInfo->uObject2FacesCount; ++i)
		{
			vNormal += (*pFaces)[pCollisionInfo->Object2Faces[i]].normal;
		}
		if(MathHelper::IsZeroVector(vNormal) == false)
			vNormal = MFD_Normalize(vNormal);

		//find the center as collision point, sum/count is not correct
		Vector3 *pPoint = nullptr;
		float32 minX = FLT_MAX;
		float32 minY = FLT_MAX;
		float32 minZ = FLT_MAX;
		float32 maxX = -FLT_MAX;
		float32 maxY = -FLT_MAX;
		float32 maxZ = -FLT_MAX;
		for(uint32 i = 0; i < pCollisionInfo->uTouchPostionCount; ++i)
		{
			pPoint = &pCollisionInfo->touchedPostions[i];

			if(pPoint->x < minX)
				minX  = pPoint->x;
			if(pPoint->x > maxX)
				maxX  = pPoint->x;

			if(pPoint->y < minY)
				minY  = pPoint->y;
			if(pPoint->y > maxY)
				maxY  = pPoint->y;

			if(pPoint->z < minZ)
				minZ  = pPoint->z;
			if(pPoint->z > maxZ)
				maxZ  = pPoint->z;
		}
		vTouchedPoint.x = (minX + maxX) * 0.5f;
		vTouchedPoint.y = (minY + maxY) * 0.5f;
		vTouchedPoint.z = (minZ + maxZ) * 0.5f;
	}
	else
	{
		if(pCollisionInfo->uTouchPostionCount == 1)
		{

			if(pCollider1->GetType() == ColliderType::Sphere && pCollider2->GetType() == ColliderType::Sphere)
			{
				//already calculated
				vTouchedPoint = pCollisionInfo->contactPoints[0].vPos;
				vNormal = pCollisionInfo->contactPoints[0].vNormal;
			}
			else
			{		
				vTouchedPoint = pCollisionInfo->touchedPostions[0];

				if(pCollisionInfo->uObject1FacesCount > 0)
					face = &(*pCollisionInfo->pObject1->GetColliderComponent()->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object1Faces[0]];
				else
					face = &(*pCollisionInfo->pObject2->GetColliderComponent()->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object2Faces[0]];

				vNormal = MFD_Normalize(face->normal);
			}
		}
		else if(pCollisionInfo->uTouchPostionCount == 2)
		{
			vTouchedPoint = (pCollisionInfo->touchedPostions[0] + pCollisionInfo->touchedPostions[1]) * 0.5f;
		
			if(pCollisionInfo->uObject1FacesCount > 0)
				face = &(*pCollisionInfo->pObject1->GetColliderComponent()->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object1Faces[0]];
			else
				face = &(*pCollisionInfo->pObject2->GetColliderComponent()->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object2Faces[0]];

			vNormal = MFD_Normalize(face->normal);
		}
		else
		{
			MathHelper::RemoveDuplicate(pCollisionInfo->Object1Faces,pCollisionInfo->uObject1FacesCount);
			MathHelper::RemoveDuplicate(pCollisionInfo->Object2Faces,pCollisionInfo->uObject2FacesCount);

			std::vector<Face> *pFaces1 = pCollisionInfo->pObject1->GetColliderComponent()->GetCollisionMesh()->GetFaces();
			std::vector<Face> *pFaces2 = pCollisionInfo->pObject2->GetColliderComponent()->GetCollisionMesh()->GetFaces();

			//check face of object1
			bool bUnifiedNormal = true;
			vNormal = (*pFaces1)[pCollisionInfo->Object1Faces[0]].normal;
			for(uint32 i = 0; i < pCollisionInfo->uObject1FacesCount; ++i)
			{
				if(MathHelper::AlmostEqual(vNormal, (*pFaces1)[pCollisionInfo->Object1Faces[i]].normal) == false)
				{
					bUnifiedNormal = false;
					break;
				}
			}

			if(bUnifiedNormal == false)
			{
				//check face of object2
				bUnifiedNormal = true;
				vNormal = (*pFaces2)[pCollisionInfo->Object2Faces[0]].normal;
				for(uint32 i = 0; i < pCollisionInfo->uObject2FacesCount; ++i)
				{
					if(MathHelper::AlmostEqual(vNormal, (*pFaces2)[pCollisionInfo->Object2Faces[i]].normal) == false)
					{
						bUnifiedNormal = false;
						break;
					}
				}

				if(bUnifiedNormal == false)
				{
					vNormal.x = vNormal.y = vNormal.z = 0;
				}
			}

			//if normal length is zero, 
			//it either a edge-edge-like collision(normal should depend on the velocity direction) 
			//or same-normal-face collsion (not really possible to check cos the face has zero thickness, so again use the velocity direction)
			//todo:find a better algorithm
			if(MathHelper::IsZeroVector(vNormal) == false)
			{
				vNormal = MFD_Normalize(vNormal);
			}
			else
			{
				//a little bit cheat code
				//if there is a static object and has only two different normals
				//then the normal will be the sum of two normals 
				std::vector<Vector3> tempList;
				uint32 uCount = 0;
				if(pCollisionInfo->pObject1->GetRigidBodyComponent()->m_bStatic &&  pCollisionInfo->uObject1FacesCount >= 2)
				{
					for(uint32 i = 0; i < pCollisionInfo->uObject1FacesCount; ++i)
					{
						tempList.push_back( (*pFaces1)[pCollisionInfo->Object1Faces[i]].normal);
					}
					uCount = tempList.size();
					MathHelper::RemoveDuplicate(tempList,uCount);
					if(uCount == 2)
					{
						vNormal = tempList[0] + tempList[1];
					}
				}
				else if(pCollisionInfo->pObject2->GetRigidBodyComponent()->m_bStatic &&  pCollisionInfo->uObject2FacesCount >= 2)
				{
					for(uint32 i = 0; i < pCollisionInfo->uObject2FacesCount; ++i)
					{
						tempList.push_back( (*pFaces2)[pCollisionInfo->Object2Faces[i]].normal);
					}
					uCount = tempList.size();
					MathHelper::RemoveDuplicate(tempList,uCount);
					if(uCount == 2)
					{
						vNormal = tempList[0] + tempList[1];
					}
				}


				if(MathHelper::IsZeroVector(vNormal) == false)
				{
					vNormal = MFD_Normalize(vNormal);
				}
			}

			//find the center as collision point, sum/count is not correct
			Vector3 *pPoint = nullptr;
			float32 minX = FLT_MAX;
			float32 minY = FLT_MAX;
			float32 minZ = FLT_MAX;
			float32 maxX = -FLT_MAX;
			float32 maxY = -FLT_MAX;
			float32 maxZ = -FLT_MAX;
			for(uint32 i = 0; i < pCollisionInfo->uTouchPostionCount; ++i)
			{
				pPoint = &pCollisionInfo->touchedPostions[i];

				if(pPoint->x < minX)
					minX  = pPoint->x;
				if(pPoint->x > maxX)
					maxX  = pPoint->x;

				if(pPoint->y < minY)
					minY  = pPoint->y;
				if(pPoint->y > maxY)
					maxY  = pPoint->y;

				if(pPoint->z < minZ)
					minZ  = pPoint->z;
				if(pPoint->z > maxZ)
					maxZ  = pPoint->z;
			}
			vTouchedPoint.x = (minX + maxX) * 0.5f;
			vTouchedPoint.y = (minY + maxY) * 0.5f;
			vTouchedPoint.z = (minZ + maxZ) * 0.5f;
		}
	}

	MathHelper::Clean(vNormal);
	MathHelper::Clean(vTouchedPoint);

	pCollisionInfo->contactPoints[0].vPos =  vTouchedPoint;
	pCollisionInfo->contactPoints[0].vNormal =  vNormal;
}

void 
CollisionManager::ProcessCollision(CollisionInfo *pCollisionInfo)
{
	//DEBUG_PRINTF(("Collision: %s <> %s\n", pCollisionInfo->pObject1->GetName(), pCollisionInfo->pObject2->GetName()));
#if DISABLE_COLLISION
	return;
#endif

	RigidBody *pCollidedBody1 = pCollisionInfo->pObject1->GetRigidBodyComponent();
	RigidBody *pCollidedBody2 = pCollisionInfo->pObject2->GetRigidBodyComponent();

	if(pCollidedBody1 == nullptr || pCollidedBody2 == nullptr || pCollidedBody1->IsEnabled() == false || pCollidedBody2->IsEnabled() == false)
		return;

	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	//fetch info
	Vector3 vTouchedPoint = pCollisionInfo->contactPoints[0].vPos;
	Vector3 vNormal = pCollisionInfo->contactPoints[0].vNormal;

	Vector3 vCenterOfMass1 = pCollidedBody1->m_vCenterOfMass_world;
	Vector3 vCenterOfMass2 = pCollidedBody2->m_vCenterOfMass_world;
	Vector3 vR1 = vTouchedPoint - vCenterOfMass1;
	Vector3 vR2 = vTouchedPoint - vCenterOfMass2;
	MathHelper::Clean(vR1);
	MathHelper::Clean(vR2);

#if USE_FRAME_SUM_VELOCITY
	Vector3 vV1 = pCollidedBody1->m_vVelocity + pCollidedBody1->m_vForce * pCollidedBody1->m_fMassInversed * fDeltaTime + MFD_Cross(pCollidedBody1->m_vAngularVelocity + pCollidedBody1->m_vTorque * pCollidedBody1->m_mMomentInertiaWorld * fDeltaTime, vR1);
	Vector3 vV2 = pCollidedBody2->m_vVelocity + pCollidedBody2->m_vForce * pCollidedBody2->m_fMassInversed * fDeltaTime + MFD_Cross(pCollidedBody2->m_vAngularVelocity + pCollidedBody2->m_vTorque * pCollidedBody2->m_mMomentInertiaWorld * fDeltaTime, vR2);
#else
	Vector3 vV1 = pCollidedBody1->m_vVelocity + MFD_Cross(pCollidedBody1->m_vAngularVelocity, vR1);
	Vector3 vV2 = pCollidedBody2->m_vVelocity + MFD_Cross(pCollidedBody2->m_vAngularVelocity, vR2);
#endif

	Vector3 vRelV = vV1 -vV2;
	MathHelper::Clean(vRelV);

	if(vRelV.x == 0 && vRelV.y == 0 && vRelV.z == 0)
	{
		//relative velocity is 0, nothing happens
		//assert(!"Really want to know if this will happen");
		return;
	}

	//if normal length is zero, 
	//it either a edge-edge-like collision(normal should depend on the velocity direction) 
	//or same-normal-face collsion (not really possible to check cos the face has zero thickness, so again use the velocity direction)
	//todo:find a better algorithm
	if(MathHelper::IsZeroVector(vNormal))
	{
		vNormal = MFD_Normalize(vRelV);
	}

	//make normal toward object1
	float32 fDot = MFD_Dot(MFD_Normalize(vR1),vNormal);
	if(fDot > 0)
	{
		vNormal = -vNormal;
	}
	MathHelper::Clean(vNormal);
	//save the final normal, so no need to calculate it again when seperate the objects
	pCollisionInfo->contactPoints[0].vNormal = vNormal;

	//start check resting
	float32 fRelVLength = MFD_Length(vRelV);
	float32 fRestingFactor = fabsf(MFD_Length(PhysicsContants::Gravity) * fDeltaTime * 1.2f); //should be fine between 1.1 - 3.0 frames
	bool bIntoResting1 = false;
	bool bIntoResting2 = false;
	if(fRelVLength < fRestingFactor)
	{

		if(pCollidedBody1->m_bStatic == false
			&& MFD_Length(pCollidedBody1->m_vVelocity) < fRestingFactor 
			&& MFD_Length(pCollidedBody1->m_vAngularVelocity) < fRestingFactor * 2)
		{
			pCollidedBody1->EnterResting();
			pCollidedBody2->AddToRelyOnList(pCollidedBody1);
			bIntoResting1 = true;
		}

		if(pCollidedBody2->m_bStatic == false
			&& MFD_Length(pCollidedBody2->m_vVelocity) < fRestingFactor 
			&& MFD_Length(pCollidedBody2->m_vAngularVelocity) < fRestingFactor * 2)
		{
			pCollidedBody2->EnterResting();
			pCollidedBody1->AddToRelyOnList(pCollidedBody2);
			bIntoResting2 = true;
		}

		if(bIntoResting1 || bIntoResting2)
			return;
	}

	float32 fVRDotN = MFD_Dot(vRelV, vNormal);

	Vector3 vR1xN = MFD_Cross(vR1,vNormal);
	Vector3 vR2xN = MFD_Cross(vR2,vNormal);

	Vector3 vIRN1 = MFD_Cross(vR1xN,vR1) * pCollidedBody1->m_mMomentInertiaWorld;
	Vector3 vIRN2 = MFD_Cross(vR2xN,vR2) * pCollidedBody2->m_mMomentInertiaWorld;

	float32 fMassInversed1 =  pCollidedBody1->m_fMassInversed;
	float32 fMassInversed2 =  pCollidedBody2->m_fMassInversed;

	bool bStaticObject = false;
	//fix for static objects
	if(pCollidedBody1->m_bStatic)
	{
		fMassInversed1 = 0;
		vIRN1.x = vIRN1.y = vIRN1.z = 0;
		bStaticObject = true;
	}
	if(pCollidedBody2->m_bStatic)
	{
		fMassInversed2 = 0;
		vIRN2.x = vIRN2.y = vIRN2.z = 0;
		bStaticObject = true;
	}

	float32 fDenom = fMassInversed1 + fMassInversed2 + MFD_Dot((vIRN1 + vIRN2),vNormal);
	float32 fElasticity1 = pCollidedBody1->m_fElasticity;
	float32 fElasticity2 = pCollidedBody2->m_fElasticity;

	if(fRelVLength < 2.0f)//disable Elasticity when velocity is low to prevent mirco shake
	{
		fElasticity1 = fElasticity2 = 0;
	}

	float32 fEnergyRemain = (1.0f - COLLISION_ENERGY_LOSS);

	//two e or single e?
	float32 fJ1 = -(1 +  pCollidedBody1->m_fElasticity) * fVRDotN / fDenom * fEnergyRemain;
	float32 fJ2 = -(1 +  pCollidedBody2->m_fElasticity) * fVRDotN / fDenom * fEnergyRemain;

	float32 fTimeFactor = 1.0f / fDeltaTime;

	Vector3 vForce1 =  vNormal * fJ1 * fTimeFactor;
	Vector3 vForce2 = -vNormal * fJ2 * fTimeFactor;
	Vector3 vTorque1 = vR1xN * fJ1 * fTimeFactor;
	Vector3 vTorque2 = -vR2xN * fJ2 * fTimeFactor;

	MathHelper::Clean(vTorque1);
	MathHelper::Clean(vTorque2);

	pCollidedBody1->AddMovementForce(vForce1);
	pCollidedBody2->AddMovementForce(vForce2);
	
	pCollidedBody1->AddTorque(vTorque1);
	pCollidedBody2->AddTorque(vTorque2);


	if(bStaticObject == false)
	{
		//both moveable objects, not to apply friction
		return;
	}


	//apply friction
	float32 fUs = pCollidedBody1->m_fStaticFriction + pCollidedBody2->m_fStaticFriction;
	float32 fUd = pCollidedBody1->m_fDynamicFriction + pCollidedBody2->m_fDynamicFriction;

	//should I clamp them to 0-1?
	float32 fJUs1 = fUs * -fJ1;
	float32 fJUs2 = fUs * -fJ2;
	float32 fJUd1 = fUd * -fJ1;
	float32 fJUd2 = fUd * -fJ2;

	Vector3 vTangent1(0,0,0); 
	Vector3 vTangent2(0,0,0); //should be the opposite direction of vTangent1

	if(fVRDotN == 0)
	{
		//it's 90 degree between relative velocity and normal, so use each body's force to calculate tangent
		//fe is the sum of all external forces, so it's should be ok to use the velocity of contact point? 
		Vector3 vFE1 = vV1;
		Vector3 vFE2 = vV2;
		float32 fFEdotN = MFD_Dot(vFE1,vNormal);
		if(fFEdotN != 0 && MathHelper::IsZeroVector(vFE1) == false)
		{
			vTangent1 = MFD_Normalize(vFE1 - fFEdotN * vNormal);
		}

		fFEdotN = MFD_Dot(vFE2,vNormal);
		if(fFEdotN != 0)
		{
			vTangent2 = MFD_Normalize(vFE2 - fFEdotN * vNormal);
		}
	}
	else
	{
		vTangent1 = vRelV - fVRDotN * vNormal;
		if(MathHelper::IsZeroVector(vTangent1) == false)
		{
			vTangent1 = MFD_Normalize(vTangent1);
			vTangent2 = vTangent1;
		}
		else
		{
			//assert(!"so, what to do? it's because the wrong normal direction");
			return;
		}
	}


	//tweak a lot in here, hope all the +- signs are still correct
	if(MathHelper::IsZeroVector(vTangent1) == false && pCollidedBody1->m_bStatic == false)
	{
		float32 fJF = -fJUd1;
		float32 fVRDotT = MFD_Dot(vRelV,vTangent1);
		float32 fVRTM = fVRDotT * pCollidedBody1->m_fMass;
		if(fVRDotT == 0 || fVRTM <  fJUs2)
		{
			//based on static friction
			fJF = -fVRTM;
		}

		Vector3 vR1xT = MFD_Cross(vR1,vTangent1);
		float32 fDenomF = pCollidedBody1->m_fMassInversed + MFD_Dot( vR1xT, pCollidedBody1->m_mMomentInertiaWorld * vR1xT );
		float32 fJR = fVRDotT / fDenomF;
		float32 fAbsF = fabsf(fJF);
		fJF = MFD_Clamp(fJR, -fAbsF, fAbsF);

		Vector3 vForce =  -vTangent1 * fJF * fTimeFactor;
		pCollidedBody1->AddMovementForce(vForce);

		Vector3 vTorque = -vR1xT * fJF * fTimeFactor;
		pCollidedBody1->AddTorque(vTorque);
	}

	if(MathHelper::IsZeroVector(vTangent2) == false && pCollidedBody2->m_bStatic == false)
	{
		float32 fJF = -fJUd2;
		float32 fVRDotT = MFD_Dot(vRelV,vTangent2);
		float32 fVRTM = fVRDotT * pCollidedBody2->m_fMass;
		if(fVRDotT == 0 || fVRTM < fJUs2)
		{
			//based on static friction
			fJF = -fVRTM;
		}

		Vector3 vR2xT = MFD_Cross(vR2,vTangent2);
		float32 fDenomF = pCollidedBody1->m_fMassInversed + MFD_Dot( vR2xT, pCollidedBody1->m_mMomentInertiaWorld * vR2xT );
		float32 fJR = -fVRDotT / fDenomF;
		float32 fAbsF = fabsf(fJF);
		fJF = MFD_Clamp(fJR, -fAbsF, fAbsF);

		Vector3 vForce =  -vTangent2 * fJF * fTimeFactor;
		pCollidedBody2->AddMovementForce(vForce);

		Vector3 vTorque = -vR2xT * fJF * fTimeFactor;
		pCollidedBody2->AddTorque(vTorque);		
	}
}

void 
CollisionManager::SeparateObject(CollisionInfo *pCollisionInfo)
{
#if DISABLE_COLLISION
	return;
#endif

	RigidBody *pBody1 = pCollisionInfo->pObject1->GetRigidBodyComponent();
	RigidBody *pBody2 = pCollisionInfo->pObject2->GetRigidBodyComponent();

	if(pBody1 == nullptr || pBody2 == nullptr || pBody1->IsEnabled() == false || pBody2->IsEnabled() == false)
		return;

	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	GameObject *pObject1 = pCollisionInfo->pObject1;
	GameObject *pObject2 = pCollisionInfo->pObject2;
	Collider *pCollider1 = pCollisionInfo->pObject1->GetColliderComponent();
	Collider *pCollider2 = pCollisionInfo->pObject2->GetColliderComponent();

	Sphere sphere1;
	Sphere sphere2;
	ColliderType eType1 = pCollider1->GetType();
	ColliderType eType2 = pCollider2->GetType();

	if(eType1 == ColliderType::Sphere)
	{
		sphere1.c = pObject1->GetWorldPos();
		sphere1.r = pCollider1->GetRange();
	}
	if(eType2 == ColliderType::Sphere)
	{
		sphere2.c = pObject2->GetWorldPos();
		sphere2.r = pCollider2->GetRange();
	}

	//setup triangles
	Vector3 metaTri1[3];
	Vector3 metaTri2[3];
	Vector3 tri1[3];
	Vector3 tri2[3];

	Face *faces1 = nullptr;
	Face *faces2 = nullptr;
	const std::vector<Vector3> *pVList1 = nullptr;
	const std::vector<Vector3> *pVList2 = nullptr;

	Matrix44 *pMat1 = pObject1->GetTranformMat();
	Matrix44 *pMat2 = pObject2->GetTranformMat();

	//first two collided faces, may not be the best face samples
	if(eType1 == ColliderType::Mesh)
	{
		faces1 = &(*pCollider1->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object1Faces[0]];
		pVList1 = pCollider1->GetCollisionMesh()->GetVertices();
		metaTri1[0] = (*pVList1)[faces1->v[0]];
		metaTri1[1] = (*pVList1)[faces1->v[1]];
		metaTri1[2] = (*pVList1)[faces1->v[2]];
		tri1[0] = Vector3((*pMat1) * Vector4(metaTri1[0],1.0f));
		tri1[1] = Vector3((*pMat1) * Vector4(metaTri1[1],1.0f));
		tri1[2] = Vector3((*pMat1) * Vector4(metaTri1[2],1.0f));
	}

	if(eType2 == ColliderType::Mesh)
	{
		faces2 = &(*pCollider2->GetCollisionMesh()->GetFaces())[pCollisionInfo->Object2Faces[0]];
		pVList2 = pCollider2->GetCollisionMesh()->GetVertices();
		metaTri2[0] = (*pVList2)[faces2->v[0]];
		metaTri2[1] = (*pVList2)[faces2->v[1]];
		metaTri2[2] = (*pVList2)[faces2->v[2]];
		tri2[0] = Vector3((*pMat2) * Vector4(metaTri2[0],1.0f));
		tri2[1] = Vector3((*pMat2) * Vector4(metaTri2[1],1.0f));
		tri2[2] = Vector3((*pMat2) * Vector4(metaTri2[2],1.0f));
	}


#if SEPARATE_BY_NORMAL
	//separate based on normal direction
	//will cause position offset when resting on a slope
	Vector3 vTouchedPoint =  pCollisionInfo->contactPoints[0].vPos;
	Vector3 vNormal1 = pCollisionInfo->contactPoints[0].vNormal;
	Vector3 vNormal2 = -vNormal1;

	Vector3 vR1 = vTouchedPoint - pBody1->m_vCenterOfMass_world;
	Vector3 vR2 = vTouchedPoint - pBody2->m_vCenterOfMass_world;
	Vector3 vV1 = pBody1->m_vVelocity + MFD_Cross(pBody1->m_vAngularVelocity, vR1);
	Vector3 vV2 = pBody2->m_vVelocity + MFD_Cross(pBody2->m_vAngularVelocity, vR2);

	if(MathHelper::IsZeroVector(vNormal1) && MathHelper::IsZeroVector(vNormal2))
	{
		//just in cause, a little bit cheat code
		Vector3 vDir = (pBody1->m_vCenterOfMass_world - pBody2->m_vCenterOfMass_world);
		
		if(MathHelper::IsZeroVector(vDir)==false)
		{
			vNormal1 = MFD_Normalize(vDir);
			vNormal2 = -vNormal1;
		}
		else
		{
			assert(!"really should not happe");
		}
	}

	//check if the normal direction will separate the object
	//this should greatly reduce the chance object sink into another object
	Plane sCollisionPlane;
	sCollisionPlane.n = vNormal1;//hope the normal is correct
	sCollisionPlane.d = MFD_Dot(vNormal1,vTouchedPoint);
	float32 fDis1 = CollisionHelper::DistPointPlane(pBody1->m_vCenterOfMass_world,sCollisionPlane);
	float32 fDis2 = CollisionHelper::DistPointPlane(pBody2->m_vCenterOfMass_world,sCollisionPlane);
	float32 fLastSum = fabsf(fDis1 + fDis2);
	Vector3 vP1 = pBody1->m_vCenterOfMass_world + vNormal1 * fabsf(fDis1) * 0.5f;//move a little bit, but won't change the distance sign
	Vector3 vP2 = pBody2->m_vCenterOfMass_world + vNormal2 * fabsf(fDis2) * 0.5f;//move a little bit, but won't change the distance sign
	float32 fNewDis1 = CollisionHelper::DistPointPlane(vP1,sCollisionPlane);
	float32 fNewDis2 = CollisionHelper::DistPointPlane(vP2,sCollisionPlane);
	float32 fNewSum = fabsf(fNewDis1 + fNewDis2);		
	if((fNewSum - fLastSum) <= 0)
	{
		vNormal1 = -vNormal1;
		vNormal2 = -vNormal2;
	}

	float32 fStep1Length = MFD_Length(vV1) * fDeltaTime * ( OVERLAP_RANGE_SEPARATE / TOTAL_SETP_SEPARATE);
	float32 fStep2Length = MFD_Length(vV2) * fDeltaTime * ( OVERLAP_RANGE_SEPARATE / TOTAL_SETP_SEPARATE);

	if(pBody1->m_bStatic == false && fStep1Length < MIN_SETP_LENGTH_SEPARATE)
		fStep1Length = MIN_SETP_LENGTH_SEPARATE;

	if(pBody2->m_bStatic == false && fStep2Length < MIN_SETP_LENGTH_SEPARATE)
		fStep2Length = MIN_SETP_LENGTH_SEPARATE;

	Vector3 vStep1 = vNormal1 * fStep1Length;
	Vector3 vStep2 = vNormal2 * fStep2Length;

	bool bSeparated = false;
	if(eType1 == ColliderType::Sphere && eType2 == ColliderType::Sphere)
	{
		for(uint32 i = 0; i < TOTAL_SETP_SEPARATE; ++i)
		{
			if(pBody1->m_bStatic == false)
			{
				sphere1.c += vStep1;
			}	
			if(pBody2->m_bStatic == false)
			{
				sphere2.c += vStep2;
			}

			if(CollisionHelper::SphereOnSphere(sphere1,sphere2) == false)
			{
				bSeparated = true;

				pObject1->SetPos(sphere1.c);
				pObject2->SetPos(sphere2.c);

				pObject1->RefreshTranformMat();
				pObject2->RefreshTranformMat();

				break;
			}
		}
	}
	else  if(eType1 == ColliderType::Sphere && eType2 == ColliderType::Mesh)
	{
		Vector3 out;
		for(uint32 i = 0; i < TOTAL_SETP_SEPARATE; ++i)
		{
			if(pBody1->m_bStatic == false)
			{
				sphere1.c += vStep1;
			}	
			if(pBody2->m_bStatic == false)
			{
				pObject2->MovePos(vStep2);
				pObject2->RefreshTranformMat();
				pMat2 = pObject2->GetTranformMat();
				tri2[0] = Vector3((*pMat2) * Vector4(metaTri2[0],1.0f));
				tri2[1] = Vector3((*pMat2) * Vector4(metaTri2[1],1.0f));
				tri2[2] = Vector3((*pMat2) * Vector4(metaTri2[2],1.0f));
			}

			if(CollisionHelper::SphereOnTriangle(sphere1,tri2[0],tri2[1],tri2[2],out) == false)
			{
				bSeparated = true;

				pObject1->SetPos(sphere1.c);
				pObject1->RefreshTranformMat();

				break;
			}
		}
	}
	else if(eType1 == ColliderType::Mesh && eType2 == ColliderType::Mesh)
	{
		for(uint32 i = 0; i < TOTAL_SETP_SEPARATE; ++i)
		{
			if(pBody1->m_bStatic == false)
			{
				pObject1->MovePos(vStep1);
				pObject1->RefreshTranformMat();
				pMat1 = pObject1->GetTranformMat();
			
				tri1[0] = Vector3((*pMat1) * Vector4(metaTri1[0],1.0f));
				tri1[1] = Vector3((*pMat1) * Vector4(metaTri1[1],1.0f));
				tri1[2] = Vector3((*pMat1) * Vector4(metaTri1[2],1.0f));
			}
	
			if(pBody2->m_bStatic == false)
			{
				pObject2->MovePos(vStep2);
				pObject2->RefreshTranformMat();
				pMat2 = pObject2->GetTranformMat();
				tri2[0] = Vector3((*pMat2) * Vector4(metaTri2[0],1.0f));
				tri2[1] = Vector3((*pMat2) * Vector4(metaTri2[1],1.0f));
				tri2[2] = Vector3((*pMat2) * Vector4(metaTri2[2],1.0f));
			}


			if(CollisionHelper::TriangleOnTriangle(tri1[0],tri1[1],tri1[2],tri2[0],tri2[1],tri2[2]) == false)
			{
				bSeparated = true;
				break;
			}
		}
	}
	else
	{
		assert(!"unhandled type match");
	}
	
#else

	//separate based on velocity and rotation direction, need to debug the rotation part
	//this should be the better way 
	Vector3 vMoveStep1 = -pBody1->m_vVelocity / (float32)TOTAL_SETP_SEPARATE * fDeltaTime;
	Vector3 vMoveStep2 = -pBody2->m_vVelocity / (float32)TOTAL_SETP_SEPARATE * fDeltaTime;

	Quaternion qRot1 = (*pObject1->GetOrientation());
	Quaternion qRot2 = (*pObject2->GetOrientation());
	Vector3 vRotStep1 = -pBody1->m_vAngularVelocity / (float32)TOTAL_SETP_SEPARATE * fDeltaTime;
	Vector3 vRotStep2 = -pBody2->m_vAngularVelocity / (float32)TOTAL_SETP_SEPARATE * fDeltaTime;
	Quaternion qRotStep1 = Quaternion(0, vRotStep1.x,  vRotStep1.y,  vRotStep1.z) * qRot1;
	Quaternion qRotStep2 = Quaternion(0, vRotStep2.x,  vRotStep2.y,  vRotStep2.z) * qRot2;
	MathHelper::Clean(qRotStep1);
	MathHelper::Clean(qRotStep2);

	bool bSeparated = false;
	for(uint32 i = 0; i < TOTAL_SETP_SEPARATE; ++i)
	{
		if(pBody1->m_bStatic == false)
		{
			pObject1->MovePos(vMoveStep1);
			pObject1->Rotate(qRotStep1);
			pObject1->RefreshTranformMat();
			//qRotStep1 =  Quaternion(0, vRotStep1.x,  vRotStep1.y,  vRotStep1.z) * (*pObject1->GetOrientation());

			pMat1 = pObject1->GetTranformMat();
			
			tri1[0] = Vector3((*pMat1) * Vector4(metaTri1[0],1.0f));
			tri1[1] = Vector3((*pMat1) * Vector4(metaTri1[1],1.0f));
			tri1[2] = Vector3((*pMat1) * Vector4(metaTri1[2],1.0f));
		}
	
		if(pBody2->m_bStatic == false)
		{
			pObject2->MovePos(vMoveStep2);
			pObject2->Rotate(qRotStep2);
			pObject2->RefreshTranformMat();
			//qRotStep2 = Quaternion(0, vRotStep2.x,  vRotStep2.y,  vRotStep2.z) * (*pObject2->GetOrientation());

			pMat2 = pObject2->GetTranformMat();
			tri2[0] = Vector3((*pMat2) * Vector4(metaTri2[0],1.0f));
			tri2[1] = Vector3((*pMat2) * Vector4(metaTri2[1],1.0f));
			tri2[2] = Vector3((*pMat2) * Vector4(metaTri2[2],1.0f));
		}

		if(CollisionHelper::TriangleOnTriangle(tri1[0],tri1[1],tri1[2],tri2[0],tri2[1],tri2[2]) == false)
		{
			bSeparated = true;
			break;
		}
	}

#endif
	//assert(bSeparated && "if not separated, thing could go wrong");
}

void 
CollisionManager::CheckCollision(Scene* pScene)
{
	int32 uObjectCount = pScene->GetObjectCount();
	std::list<GameObject*> &objects = *pScene->GetObjectList();
	int32 iMax = uObjectCount - 1;
	int32 i = 0;
	int32 j = 0;
	GameObject* pObj1 = nullptr;
	GameObject* pObj2 = nullptr;
	Collider* pCollider1 = nullptr;
	Collider* pCollider2 = nullptr;
	RigidBody* pRigidBody1 = nullptr;
	RigidBody* pRigidBody2 = nullptr;
	float32 fTotalRange;
	Vector3 vD;

	CollisionInfoCollection *pCIC =  pScene->GetCollisionInfoCollection();
	
	assert(pCIC != nullptr && "Scene::InitCollisionCollection is not called");
	if(pCIC == nullptr)
		return;

	bool bWillNotMove1 = false;
	bool bWillNotMove2 = false;
	std::list<GameObject*>::iterator it1,it2;
	for(it1 = objects.begin(); it1 != objects.end(); ++it1)
	{
		pObj1 = *it1;
		pCollider1 = pObj1->GetColliderComponent();		
		pRigidBody1 = pObj1->GetRigidBodyComponent();
		if(pCollider1 == nullptr || pCollider1->IsEnabled() == false)
			continue;

		for(it2 = it1; it2 != objects.end(); ++it2)
		{
			if(*it1 == *it2)
				continue;

			pObj2 = *it2;
			pCollider2 = pObj2->GetColliderComponent();		
			pRigidBody2 = pObj2->GetRigidBodyComponent();
			if(pCollider2 == nullptr || pCollider2->IsEnabled() == false)
				continue;

			//if both objects did not moved, ignore them
			if(pObj1->IsMoved()==false && pObj2->IsMoved()==false)
				continue;

			if(pRigidBody1 != nullptr && pRigidBody2 != nullptr && pRigidBody1->m_bStatic && pRigidBody2->m_bStatic)
				continue;

			vD = pObj2->GetWorldPos() - pObj1->GetWorldPos();
			fTotalRange = pCollider1->GetRange() + pCollider2->GetRange();

			if(MFD_Dot(vD,vD) > (fTotalRange * fTotalRange))
				continue;//no chance to collide in logic distance

			if(IsColliderCollided(pObj1,pObj2, pCIC->collsionInfoList[pCIC->uCount]))
			{
				pCIC->uCount += 1;
			}

			assert(pCIC->uCount < pCIC->collsionInfoList.size() && "call Scene::InitCollisionCollection with a larger collection size");
		}
	}

}

bool 
CollisionManager::IsRayColliderCollided(Vector3& vRayPos, Vector3& vRayDir, float32 fMaxRayLength, GameObject* pObject, Vector3& pOutHitPos)
{
	Collider* pCollider = pObject->GetColliderComponent();
	if(pCollider == nullptr || pCollider->IsEnabled() == false)
		return false;

	ColliderType eType = pCollider->GetType();
	if(eType == ColliderType::Mesh)
	{
		//do a quick range test first
		Sphere sphere;
		sphere.c = pObject->GetWorldPos();
		sphere.r = pCollider->GetRange();
		if(CollisionHelper::RayOnSphere(vRayPos,vRayDir,sphere) == false)
		{
			//no chance in logic
			return false;
		}

		//now check all the trangles
		Vector3 vRayEnd = vRayPos + vRayDir * fMaxRayLength;
		const std::vector<Vector3> &pVertices = *pCollider->GetVerticesCache();
		CollisionMesh* pMesh = pCollider->GetCollisionMesh();
		int32 nMeshFaceCount = pMesh->GetFaceCount();
		Face* sFace;

		Vector3 vHitPos;
		Vector3 vTemp;
		float32 fT = 0;
		float32 fMinDis = FLT_MAX;
		float32 fDis = 0;
		bool bHit = false;
		uint32 uFaceIndex = 0;
		for(int32 i = 0; i < nMeshFaceCount; ++i)
		{
			sFace = &(pMesh->m_Faces[i]);

			if(CollisionHelper::SegmentOnTriangle(vRayPos,vRayEnd,pVertices[sFace->v[0]],pVertices[sFace->v[1]],pVertices[sFace->v[2]],vHitPos,fT))
			{
				vTemp = vRayPos - pOutHitPos;
				fDis = MFD_Dot(vTemp,vTemp);
				if(fDis < fMinDis)
				{
					bHit = true;
					fMinDis = fDis;
					pOutHitPos = vHitPos;
				}
			}		
		}

		return bHit;
	}
	else if(eType == ColliderType::Sphere)
	{
		float32 fT = 0;
		Sphere sphere;
		sphere.c = pObject->GetWorldPos();
		sphere.r = pCollider->GetRange();
		return CollisionHelper::RayOnSphere(vRayPos,vRayDir,sphere,fT,pOutHitPos);
		
	}
	else
	{
		assert(!"unhandled type match");
		return false;
	}
}

bool 
CollisionManager::IsColliderCollided(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo)
{
	Collider* pCollider1 = pObject1->GetColliderComponent();
	Collider* pCollider2 = pObject2->GetColliderComponent();

	if(pCollider1 == nullptr || pCollider2 == nullptr || pCollider1->IsEnabled() == false || pCollider2->IsEnabled() == false)
		return false;

	ColliderType eType1 = pCollider1->GetType();
	ColliderType eType2 = pCollider2->GetType();

	if(eType1 == ColliderType::Mesh && eType2 == ColliderType::Mesh)
	{
		return MeshColliderToMeshCollider(pObject1,pObject2,outInfo);
	}
	else if(eType1 == ColliderType::Sphere && eType2 == ColliderType::Sphere)
	{
		return SphereColliderToSphereCollider(pObject1,pObject2,outInfo);
	}
	if(eType1 == ColliderType::Sphere && eType2 == ColliderType::Mesh)
	{
		return SphereColliderToMeshCollider(pObject1,pObject2,outInfo,false);
	}
	if(eType1 == ColliderType::Mesh && eType2 == ColliderType::Sphere)
	{
		//order might not important, cannot think up why I need to care at this moment, but just in case
		return SphereColliderToMeshCollider(pObject2,pObject1,outInfo,true);
	}
	else
	{
		assert(!"unhandled type match");
		return false;
	}

}

bool 
CollisionManager::MeshColliderToMeshCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo)
{
	Collider* pCollider1 = pObject1->GetColliderComponent();
	Collider* pCollider2 = pObject2->GetColliderComponent();

	if(pCollider1 == nullptr || pCollider2 == nullptr || pCollider1->IsEnabled() == false || pCollider2->IsEnabled() == false)
		return false;

	const std::vector<Vector3> &pVertices1 = *pCollider1->GetVerticesCache();
	const std::vector<Vector3> &pVertices2 = *pCollider2->GetVerticesCache();

	CollisionMesh* pMesh1 = pCollider1->GetCollisionMesh();
	CollisionMesh* pMesh2 = pCollider2->GetCollisionMesh();
	int32 nMesh1FaceCount = pMesh1->GetFaceCount();
	int32 nMesh2FaceCount = pMesh2->GetFaceCount();
	Face* sFace1;
	Face* sFace2;
	int32 i = 0;
	int32 j = 0;

	bool bHit = false;
	uint32 uMaxTouchCount = outInfo.touchedPostions.size() - 1;//2 per check
	uint32 uFaceIndex = 0;
	for(i = 0; i < nMesh1FaceCount; ++i)
	{
		sFace1 = &(pMesh1->m_Faces[i]);
		for(j = 0; j < nMesh2FaceCount; ++j)
		{
			sFace2 = &(pMesh2->m_Faces[j]);
			if(outInfo.uTouchPostionCount < uMaxTouchCount
				&& CollisionHelper::TriangleOnTriangle(pVertices1[sFace1->v[0]],pVertices1[sFace1->v[1]],pVertices1[sFace1->v[2]], 
													pVertices2[sFace2->v[0]],pVertices2[sFace2->v[1]],pVertices2[sFace2->v[2]],
													outInfo.touchedPostions[outInfo.uTouchPostionCount],outInfo.touchedPostions[outInfo.uTouchPostionCount + 1]))
			{
				bHit = true;				

				outInfo.Object1Faces[outInfo.uObject1FacesCount] = i;
				outInfo.Object2Faces[outInfo.uObject2FacesCount] = j;
				
				outInfo.uTouchPostionCount += 2;
				outInfo.uObject1FacesCount += 1;
				outInfo.uObject2FacesCount += 1;
			}
		}
	}


	if(bHit)
	{
		outInfo.pObject1 = pObject1;
		outInfo.pObject2 = pObject2;
	}

	outInfo.bCollided = bHit;

	return bHit;
}

bool 
CollisionManager::SphereColliderToMeshCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo, bool bSwapObjects)
{
	Collider* pCollider1 = pObject1->GetColliderComponent();
	Collider* pCollider2 = pObject2->GetColliderComponent();

	if(pCollider1 == nullptr || pCollider2 == nullptr || pCollider1->IsEnabled() == false || pCollider2->IsEnabled() == false)
		return false;

	Sphere sphere;
	sphere.c = pObject1->GetWorldPos();
	sphere.r = pCollider1->GetRange();

	const std::vector<Vector3> &pVertices = *pCollider2->GetVerticesCache();

	CollisionMesh* pMesh = pCollider2->GetCollisionMesh();
	int32 nMeshFaceCount = pMesh->GetFaceCount();
	Face* sFace;

	bool bHit = false;
	uint32 uMaxTouchCount = outInfo.touchedPostions.size();
	uint32 uFaceIndex = 0;
	for(int32 i = 0; i < nMeshFaceCount; ++i)
	{
		sFace = &(pMesh->m_Faces[i]);

		if(outInfo.uTouchPostionCount < uMaxTouchCount
			&& CollisionHelper::SphereOnTriangle(sphere, //sphere
											pVertices[sFace->v[0]],pVertices[sFace->v[1]],pVertices[sFace->v[2]], //trangle
											outInfo.touchedPostions[outInfo.uTouchPostionCount]))//output
		{

				bHit = true;

				outInfo.uTouchPostionCount += 1;
				outInfo.Object2Faces[outInfo.uObject2FacesCount] = i;
				outInfo.uObject2FacesCount += 1;
		}		
	}


	if(bHit)
	{
		outInfo.pObject1 = pObject1;
		outInfo.pObject2 = pObject2;
	}

	outInfo.bCollided = bHit;

	return bHit;
}

bool 
CollisionManager::SphereColliderToSphereCollider(GameObject* pObject1, GameObject* pObject2, CollisionInfo& outInfo)
{
	Collider* pCollider1 = pObject1->GetColliderComponent();
	Collider* pCollider2 = pObject2->GetColliderComponent();

	
	if(pCollider1 == nullptr || pCollider2 == nullptr || pCollider1->IsEnabled() == false || pCollider2->IsEnabled() == false)
		return false;

	Vector3 vPos1 = pObject1->GetWorldPos();
	Vector3 vPos2 = pObject2->GetWorldPos();
	float32 fRange1 = pCollider1->GetRange();
	float32 fRange2 = pCollider2->GetRange();

	Vector3 vD = vPos1 - vPos2;

	float32 fTotalRange = pCollider1->GetRange() + pCollider2->GetRange();

	if((vD.x * vD.x + vD.y * vD.y + vD.z * vD.z) <= (fTotalRange * fTotalRange))
	{
		float32 fD = MFD_Length(vD);
		if(fD == 0)
		{
			//wrong, should not happen or two objects are totally overlapped
			outInfo.bCollided = false;
			return false;
		}

		float32 fPen = fTotalRange - fD;

		outInfo.contactPoints[0].vNormal = MFD_Normalize(vD);
		outInfo.contactPoints[0].vPos = vPos2 + outInfo.contactPoints[0].vNormal * (fRange2 - fPen * 0.5f);

		outInfo.pObject1 = pObject1;
		outInfo.pObject2 = pObject2;
		outInfo.uTouchPostionCount = 1;
		outInfo.bCollided = true;
		return true;
	}
	else
	{
		outInfo.bCollided = false;
		return false;
	}

}

bool 
CollisionManager::IsPlaneColliderCollided(GameObject* pObject, PlaneHitInfo& outInfo)
{
	Collider* pCollider = pObject->GetColliderComponent();

	if(pCollider == nullptr || pCollider->IsEnabled() == false)
		return false;

	outInfo.clear();

	const std::vector<Vector3> &pVertices = *pCollider->GetVerticesCache();

	CollisionMesh* pMesh = pCollider->GetCollisionMesh();
	if(pMesh == nullptr)
		return false;

	int32 nMeshFaceCount = pMesh->GetFaceCount();
	Face* sFace;

	bool bHit = false;
	uint32 uMaxTouchCount = outInfo.touchedPostions.size();
	uint32 uFaceIndex = 0;
	uint32 uSingleSideIndex = 0;
	for(int32 i = 0; i < nMeshFaceCount; ++i)
	{
		sFace = &(pMesh->m_Faces[i]);

		if(outInfo.uTouchPostionCount < uMaxTouchCount
			&& CollisionHelper::TriangleOnPlane(pVertices[sFace->v[0]],pVertices[sFace->v[1]], pVertices[sFace->v[2]], outInfo.sPlane,
												outInfo.touchedPostions[outInfo.uTouchPostionCount],outInfo.touchedPostions[outInfo.uTouchPostionCount+1],
												outInfo.touchedPostionsT[outInfo.uTouchPostionCount],outInfo.touchedPostionsT[outInfo.uTouchPostionCount+1],uSingleSideIndex))//output
		{

				bHit = true;

				outInfo.ObjectFaces[outInfo.uObjectFacesCount] = i;
				outInfo.ObjectFaceSingleSides[outInfo.uObjectFacesCount] = uSingleSideIndex;

				outInfo.uObjectFacesCount += 1;
				outInfo.uTouchPostionCount += 2;

				sFace->bTouched = true;
		}		
		else
		{
				sFace->bTouched = false;
		}
	}


	if(bHit)
	{
		outInfo.pObject = pObject;
	}

	return bHit;
}

bool 
CollisionManager::IsPlaneMeshCollided(GameObject* pObject, PlaneHitInfo& outInfo)
{
	Model* pModel = pObject->GetModelComponent();

	if(pModel == nullptr || pModel->IsEnabled() == false)
		return false;

	outInfo.clear();

	const std::vector<Vector3> &pVertices = *pModel->GetVerticesCache();

	Mesh* pMesh = pModel->GetMesh();
	int32 nMeshFaceCount = pMesh->GetFaceCount();
	Face* sFace;

	bool bHit = false;
	uint32 uMaxTouchCount = outInfo.touchedPostions.size();
	uint32 uFaceIndex = 0;
	uint32 uSingleSideIndex = 0;
	for(int32 i = 0; i < nMeshFaceCount; ++i)
	{
		sFace = &(pMesh->m_Faces[i]);

		if(outInfo.uTouchPostionCount < uMaxTouchCount
			&& CollisionHelper::TriangleOnPlane(pVertices[sFace->v[0]],pVertices[sFace->v[1]], pVertices[sFace->v[2]], outInfo.sPlane,
												outInfo.touchedPostions[outInfo.uTouchPostionCount],outInfo.touchedPostions[outInfo.uTouchPostionCount+1],
												outInfo.touchedPostionsT[outInfo.uTouchPostionCount],outInfo.touchedPostionsT[outInfo.uTouchPostionCount+1],uSingleSideIndex))//output
		{

				bHit = true;

				outInfo.ObjectFaces[outInfo.uObjectFacesCount] = i;
				outInfo.ObjectFaceSingleSides[outInfo.uObjectFacesCount] = uSingleSideIndex;

				outInfo.uTouchPostionCount += 2;
				outInfo.uObjectFacesCount += 1;

				sFace->bTouched = true;
		}
		else
		{
				sFace->bTouched = false;
		}
	}


	if(bHit)
	{
		outInfo.pObject = pObject;
	}

	return bHit;
}

bool 
CollisionManager::IsMeshCollided(GameObject* pObject1, GameObject* pObject2)
{
	if(pObject1 == nullptr || pObject2 == nullptr)
		return false;

	Model* pModel1 = pObject1->GetModelComponent();
	Model* pModel2 = pObject2->GetModelComponent();

	if(pModel1 == nullptr || pModel2 == nullptr)
		return false;

	const std::vector<Vector3> *pMode1Vertices = pModel1->GetVerticesCache();
	const std::vector<Vector3> *pMode2Vertices = pModel2->GetVerticesCache();

	//clock_t startTime = clock();
	Mesh* pMesh1 = pModel1->GetMesh();
	Mesh* pMesh2 = pModel2->GetMesh();
	int32 nMesh1FaceCount = pMesh1->GetFaceCount();
	int32 nMesh2FaceCount = pMesh2->GetFaceCount();
	Face* sFace1;
	Face* sFace2;
	int32 i = 0;
	int32 j = 0;

	for(i = 0; i < nMesh1FaceCount; ++i)
	{
		sFace1 = &(pMesh1->m_Faces[i]);
		for(j = 0; j < nMesh2FaceCount; ++j)
		{
			sFace2 = &(pMesh2->m_Faces[j]);
			if(CollisionHelper::TriangleOnTriangle((*pMode1Vertices)[sFace1->v[0]],(*pMode1Vertices)[sFace1->v[1]],(*pMode1Vertices)[sFace1->v[2]], 
													(*pMode2Vertices)[sFace2->v[0]],(*pMode2Vertices)[sFace2->v[1]],(*pMode2Vertices)[sFace2->v[2]]))
			{
				return true;
			}
		}
	}

	//clock_t endTime = clock();
	//printf("\nHit Test cost = %d",endTime - startTime);



	return false;
}



//////////////////////////////
//  Multi Thread functions  //
//////////////////////////////

//for quick test
bool g_bMeshTest_Fast_Found = false;

void 
MeshTest_Fast_ForMT(Model* pModel1, Model* pModel2, uint32 uStartIndex, uint32 uEndIndex)
{
	Mesh* pMesh1 = pModel1->GetMesh();
	Mesh* pMesh2 = pModel2->GetMesh();
	const std::vector<Vector3> *pMode1Vertices = pModel1->GetVerticesCache();
	const std::vector<Vector3> *pMode2Vertices = pModel2->GetVerticesCache();

	uint32 nMesh2FaceCount = pMesh2->GetFaceCount();
	const std::vector<Face> *PFaces1 = pMesh1->GetFaces();
	const std::vector<Face> *PFaces2 = pMesh2->GetFaces();
	const Face* sFace1;
	const Face* sFace2;
	uint32 i = uStartIndex; 
	uint32 j = 0;
	for(; i < uEndIndex; ++i)
	{
		if(g_bMeshTest_Fast_Found)
			break;

		sFace1 = &((*PFaces1)[i]);
		for(j = 0;j < nMesh2FaceCount; ++j)
		{
			if(g_bMeshTest_Fast_Found)
				break;

			sFace2 = &((*PFaces2)[j]);
			if(CollisionHelper::TriangleOnTriangle((*pMode1Vertices)[sFace1->v[0]],(*pMode1Vertices)[sFace1->v[1]],(*pMode1Vertices)[sFace1->v[2]], 
													(*pMode2Vertices)[sFace2->v[0]],(*pMode2Vertices)[sFace2->v[1]],(*pMode2Vertices)[sFace2->v[2]]))
			{
				g_bMeshTest_Fast_Found = true;
			}
		}
	}
}

bool CollisionManager::IsMeshCollided_MT(GameObject* pObject1, GameObject* pObject2)
{
	//clock_t startTime = clock();

	Model* pModel1 = pObject1->GetModelComponent();
	Model* pModel2 = pObject2->GetModelComponent();

	if(pModel1 == nullptr || pModel2 == nullptr)
		return false;

	g_bMeshTest_Fast_Found = false;
	std::vector<std::thread> threads;

	uint32 uThreadCount = 4;
	uint32 uFaceCount = pModel1->GetMesh()->GetFaceCount();
	uint32 uStep = uFaceCount / uThreadCount + 1;

	for(uint32 i = 0; i < uThreadCount; ++i)
	{
		uint32 uStart = i * uStep;
		if(uStart >= uFaceCount)
			continue;

		uint32 uEnd = uStart + uStep;
		if(uEnd > uFaceCount)
			uEnd = uFaceCount;

		threads.push_back(std::thread(MeshTest_Fast_ForMT,pModel1,pModel2,uStart,uEnd));
	}

	
	for(auto& thread : threads)
	{
		 thread.join(); 
	} 


	//clock_t endTime = clock();
	//printf("\nHit Test cost = %d",endTime - startTime);



	return g_bMeshTest_Fast_Found;
}


//for detail test
void MeshTest_Detail()
{

}