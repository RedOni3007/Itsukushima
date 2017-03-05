#include "Itsukushima_include.h"
#include "CutBlade.h"
#include "CutPieceController.h"


//in degree
#define MAX_ROLL 100.0f
#define MAX_YAW 60.0f

#define CUT_MAX_TOUCH_POINT 2048
#define AFTER_CUT_OFFSET_LENGTH 0.001f

CutBlade::CutBlade()
{
	m_pHolderObject = nullptr;
	m_pHolderObject_Yaw = nullptr;
	m_pHolderObject_Roll = nullptr;
	m_fSwingTime = 0.25f;
	m_fSwingCD = 0.0f;
	m_fSwingSpeed = 12.0f;
	m_bInSwing = false;
	m_bComplexCut = true;

	m_fObjectStayTime = 10.0f;

	m_fRollSpeed = 0.2f;
	m_fYawSpeed = 0.1f;

	m_fRoll = 0;
	m_fYaw = 0;

	m_touchedObjects.reserve(100);
	m_newObjects.reserve(100);
}


CutBlade::~CutBlade()
{
}

void 
CutBlade::Init()
{
	m_pHitInfo = new PlaneHitInfo();
	m_pHitInfo->touchedPostions.resize(CUT_MAX_TOUCH_POINT);
	m_pHitInfo->touchedPostionsT.resize(CUT_MAX_TOUCH_POINT);
	m_pHitInfo->ObjectFaces.resize(CUT_MAX_TOUCH_POINT/2);
	m_pHitInfo->ObjectFaceSingleSides.resize(CUT_MAX_TOUCH_POINT/2);
}

void 
CutBlade::SetHolderObecjt(GameObject* pHolder)
{
	m_pHolderObject = pHolder;
}

void 
CutBlade::SetHolderObecjt_Yaw(GameObject* pHolder)
{
	m_pHolderObject_Yaw = pHolder;
}

void 
CutBlade::SetHolderObecjt_Roll(GameObject* pHolder)
{
	m_pHolderObject_Roll = pHolder;
}

void 
CutBlade::PreLogicUpdate()
{

}

void 
CutBlade::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	ProcessInput();
	
	if(m_bInSwing)
	{
		Vector3 vRight =  m_pHolderObject->GetRight();
		m_fSwingCD -= fDeltaTime;
		m_pHolderObject->Pitch(m_fSwingSpeed);

		if(m_fSwingCD < 0)
		{
			EndSwing();
		}
	}
}

void 
CutBlade::PostLogicUpdate()
{

}

void 
CutBlade::GraphicUpdate()
{

}

const char* 
CutBlade::GetName()
{
	return "CutBlade";
}

const char* 
CutBlade::ClassName()
{
	return "CutBlade";
}


void 
CutBlade::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	if(m_bInSwing == false)
		return;

	RigidBody *pRigidBody = pCollidedObj->GetRigidBodyComponent();

	if(pRigidBody == nullptr || pRigidBody->IsStatic())
		return;

	if(pCollidedObj->GetModelComponent() == nullptr)
		return;


	bool bAlreadyTouched = false;
	for(uint32 i = 0; i < m_touchedObjects.size(); ++i)
	{
		if(m_touchedObjects[i] == pCollidedObj)
		{
			bAlreadyTouched = true;
			break;
		}
	}

	if(bAlreadyTouched == false)
	{
		if(CutObject(pCollidedObj))
		{
			pCollidedObj->GetColliderComponent()->Enable(false);
			m_touchedObjects.push_back(pCollidedObj);
		}
	}
}

void 
CutBlade::ProcessInput()
{
	VPad pad = InputManager::Instance()->GetPad(0);
	Vector2 vMouseMovement = InputManager::Instance()->GetMouseMovement();

	if(pad.IsButtonPressed(VPAD_BUTTON_MOUSE1))
	{
		StartSwing();
	}
	else if(pad.IsButtonDown(VPAD_BUTTON_MOUSE2))
	{
		if(m_bInSwing == false)
		{
			float32 fRoll = vMouseMovement.x * m_fRollSpeed;
			float32 fYaw = vMouseMovement.y * m_fYawSpeed;

			m_fRoll += fRoll;
			m_fYaw += fYaw;

			if(m_fRoll > MAX_ROLL)
			{
				fRoll += MAX_ROLL - m_fRoll;
				m_fRoll = MAX_ROLL;
			}
			else if(m_fRoll < -MAX_ROLL)
			{
				fRoll -= MAX_ROLL + m_fRoll;
				m_fRoll = -MAX_ROLL;
			}

			if(m_fYaw > MAX_YAW)
			{
				fYaw += MAX_YAW - m_fYaw;
				m_fYaw = MAX_YAW;
			}
			else if(m_fYaw < -MAX_YAW)
			{
				fYaw -= MAX_YAW + m_fYaw;
				m_fYaw = -MAX_YAW;
			}

			if(fRoll != 0)
				m_pHolderObject_Roll->Roll(fRoll);

			if(fYaw != 0)
				m_pHolderObject_Yaw->Yaw(fYaw);
		}
	}
}

void 
CutBlade::StartSwing()
{
	if(m_bInSwing)
		return;

	m_bInSwing = true;
	m_fSwingCD = m_fSwingTime;
	m_qOriginOrientation = m_pHolderObject->GetOrientation();
}

void 
CutBlade::EndSwing()
{
	m_bInSwing = false;
	m_pHolderObject->SetOrientation(m_qOriginOrientation);

	Scene* pScene = m_pGameObject->GetScene();
	
	for(uint32 i = 0; i < m_touchedObjects.size(); ++i)
	{
		m_touchedObjects[i]->Destroy();
	}
	m_touchedObjects.clear();

	for(uint32 i = 0; i < m_newObjects.size(); ++i)
	{
		pScene->AddObject(*m_newObjects[i]);
	}
	m_newObjects.clear();
}

bool 
CutBlade::CutObject(GameObject* pObject)
{
	bool bToProcess = false;
	m_pHitInfo->sPlane.n = MFD_Normalize(m_pGameObject->GetWorldRight());
	m_pHitInfo->sPlane.d = MFD_Dot(m_pHitInfo->sPlane.n,m_pGameObject->GetWorldPos());

	//check if the display mesh is touched
	if(CollisionManager::Instance()->IsPlaneMeshCollided(pObject, *m_pHitInfo))
	{
		bToProcess = true;

		std::list<GameObject*> posSideObjects;
		std::list<GameObject*> negSideObjects;

		if(m_bComplexCut)
		{
			CutHelper::CutObject_complex(pObject,m_pHitInfo,posSideObjects,negSideObjects);
		}
		else
		{
			CutHelper::CutObject(pObject,m_pHitInfo,posSideObjects,negSideObjects);
		}

		
		float32 fDestroyDelay = m_fObjectStayTime;
		CutPieceController *pPieceController = (CutPieceController *)pObject->GetComponent(CutPieceController::ClassName());
		if(pPieceController != nullptr)
			fDestroyDelay = pPieceController->GetDestroyCountDown();

		std::list<GameObject*>::iterator it;
		for(it = posSideObjects.begin(); it != posSideObjects.end(); ++it)
		{
			pPieceController = new CutPieceController();
			pPieceController->SetDestroyDelay(fDestroyDelay);
			(*it)->MovePos(m_pHitInfo->sPlane.n * AFTER_CUT_OFFSET_LENGTH);
			(*it)->AddComponent(pPieceController);
			m_newObjects.push_back(*it);
		}
		for(it = negSideObjects.begin(); it != negSideObjects.end(); ++it)
		{
			pPieceController = new CutPieceController();
			pPieceController->SetDestroyDelay(fDestroyDelay);
			(*it)->MovePos(m_pHitInfo->sPlane.n * -AFTER_CUT_OFFSET_LENGTH);
			(*it)->AddComponent(pPieceController);
			m_newObjects.push_back(*it);
		}
	}
	else
	{
		//not really touched display mesh, so ignore this hit
		bToProcess = false;
	}

	return bToProcess;
}

void 
CutBlade::SwitchCutMethod()
{
	m_bComplexCut = !m_bComplexCut;

	if(m_bComplexCut)
		m_pGameObject->GetModelComponent()->SetMaterial("Solid_red");
	else
		m_pGameObject->GetModelComponent()->SetMaterial("Solid_blue");
}