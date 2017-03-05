#include "Itsukushima_include.h"
#include "CutBladeObjControl.h"

CutBladeObjControl::CutBladeObjControl()
{
	m_fMoveSpeed = 3.45f;
	m_fTurnSpeed = 3.14159f * 20;
}


CutBladeObjControl::~CutBladeObjControl()
{
}

void 
CutBladeObjControl::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

void 
CutBladeObjControl::PreLogicUpdate()
{

}

void 
CutBladeObjControl::LogicUpdate()
{
	ProcessInput();
}

void 
CutBladeObjControl::PostLogicUpdate()
{

}

void 
CutBladeObjControl::GraphicUpdate()
{

}

const char* 
CutBladeObjControl::GetName()
{
	return "CutBladeObjControl";
}

const char* 
CutBladeObjControl::ClassName()
{
	return "CutBladeObjControl";
}


void 
CutBladeObjControl::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{

}

void 
CutBladeObjControl::ProcessInput()
{
	VPad pad = InputManager::Instance()->GetPad(0);
	Vector2 vMouseMovement = InputManager::Instance()->GetMouseMovement();
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	if(pad.IsButtonDown(VPAD_BUTTON_LEFT))
	{
		MoveLR(m_fMoveSpeed * 1.0f * fDeltaTime);
	}
	
	if(pad.IsButtonDown(VPAD_BUTTON_RIGHT))
	{
		MoveLR(m_fMoveSpeed * -1.0f * fDeltaTime);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_UP))
	{
		MoveFB(m_fMoveSpeed * 1.0f * fDeltaTime);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_DOWN))
	{
		MoveFB(m_fMoveSpeed * -1.0f * fDeltaTime);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_L1))
	{
		TurnLR(m_fTurnSpeed * 1.0f * fDeltaTime);
	}
	
	if(pad.IsButtonDown(VPAD_BUTTON_L2))
	{
		TurnLR(m_fTurnSpeed * -1.0f * fDeltaTime);
	}

}

void 
CutBladeObjControl::MoveFB(float32 fValue)
{
	m_pGameObject->MoveFB(fValue);
}

void CutBladeObjControl::MoveLR(float32 fValue)
{
	m_pGameObject->MoveLR(fValue);
}

void 
CutBladeObjControl::TurnLR(float32 fValue)
{
	m_pGameObject->Yaw(fValue);
}