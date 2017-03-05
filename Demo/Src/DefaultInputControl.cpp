#include "Itsukushima_include.h"
#include "DefaultInputControl.h"


DefaultInputControl::DefaultInputControl()
{
	m_fMoveSpeed = 6.0f;
	m_fTurnSpeed = 3.14159f * 30;
}


DefaultInputControl::~DefaultInputControl()
{

}

void 
DefaultInputControl::PreLogicUpdate()
{

}

void 
DefaultInputControl::LogicUpdate()
{
	ProcessInput();
}

void 
DefaultInputControl::PostLogicUpdate()
{

}

void 
DefaultInputControl::GraphicUpdate()
{

}

const char* 
DefaultInputControl::GetName()
{
	return "DefaultInputControl";
}

const char* 
DefaultInputControl::ClassName()
{
	return "DefaultInputControl";
}


void 
DefaultInputControl::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{

}

void 
DefaultInputControl::ProcessInput()
{
	VPad pad = InputManager::Instance()->GetPad(0);
	Vector2 vMouseMovement = InputManager::Instance()->GetMouseMovement();
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	if(pad.IsButtonDown(VPAD_BUTTON_UP))
	{
		MoveFB(m_fMoveSpeed * 1.0f * fDeltaTime);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_DOWN))
	{
		MoveFB(m_fMoveSpeed * -1.0f * fDeltaTime);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_LEFT))
	{
		TurnLR(m_fTurnSpeed * 1.0f * fDeltaTime);
	}
	
	if(pad.IsButtonDown(VPAD_BUTTON_RIGHT))
	{
		TurnLR(m_fTurnSpeed * -1.0f * fDeltaTime);
	}

}

void 
DefaultInputControl::MoveFB(float32 fValue)
{
	m_pGameObject->MoveFB(fValue);
}

void DefaultInputControl::MoveLR(float32 fValue)
{
	m_pGameObject->MoveLR(fValue);
}

void 
DefaultInputControl::TurnLR(float32 fValue)
{
	m_pGameObject->Yaw(fValue);
}