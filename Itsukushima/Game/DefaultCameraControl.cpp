#include "DefaultCameraControl.h"
#include <Input/InputManager.h>
#include <Game/Camera.h>
#include <Math/MathHelper.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

DefaultCameraControl::DefaultCameraControl(void)
{
	m_fMoveSpeed = 1.0f;
	m_fTurnSpeed = 1.0f;

	m_pCamera = nullptr;

	m_vDefaultLookingDir = Vector3(0,0,-1);
}


DefaultCameraControl::~DefaultCameraControl(void)
{
	m_pCamera = nullptr;
}

void 
DefaultCameraControl::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

void 
DefaultCameraControl::PreLogicUpdate()
{
	ProcessInput();
}

void 
DefaultCameraControl::LogicUpdate()
{

}

void 
DefaultCameraControl::PostLogicUpdate()
{

}

const char* 
DefaultCameraControl::GetName()
{
	return "DefaultCameraControl";
}

const char* 
DefaultCameraControl::ClassName()
{
	return "DefaultCameraControl";
}

void 
DefaultCameraControl::ProcessInput()
{
	if(m_bActive == false)
		return;

	VPad pad = InputManager::Instance()->GetPad(0);
	Vector2 vMouseMovement = InputManager::Instance()->GetMouseMovement();

	if(pad.IsButtonDown(VPAD_BUTTON_LEFT))
	{
		MoveLR(m_fMoveSpeed * 1.0f);
	}
	
	if(pad.IsButtonDown(VPAD_BUTTON_RIGHT))
	{
		MoveLR(m_fMoveSpeed * -1.0f);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_UP))
	{
		MoveFB(m_fMoveSpeed * 1.0f);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_DOWN))
	{
		MoveFB(m_fMoveSpeed * -1.0f);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_L1))
	{
		MoveUD(m_fMoveSpeed * 1.0f);
	}

	if(pad.IsButtonDown(VPAD_BUTTON_L2))
	{
		MoveUD(m_fMoveSpeed * -1.0f);
	}

	if(vMouseMovement.x != 0 && pad.IsButtonDown(VPAD_BUTTON_MOUSE2))
	{
		Yaw(vMouseMovement.x * m_fTurnSpeed);
	}

	if(vMouseMovement.y != 0 && pad.IsButtonDown(VPAD_BUTTON_MOUSE2))
	{
		Pitch(-vMouseMovement.y * m_fTurnSpeed);
	}
}

void DefaultCameraControl::MoveFB(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vPos = m_pGameObject->GetWorldPos();
	vPos = vPos - m_pGameObject->GetForward() * fValue;
	m_pGameObject->SetPos(vPos);
}

void DefaultCameraControl::MoveUD(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vPos = m_pGameObject->GetWorldPos();
	vPos = vPos + m_pCamera->GetUp() * fValue;
	m_pGameObject->SetPos(vPos);
}

void DefaultCameraControl::MoveLR(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vPos = m_pGameObject->GetWorldPos();
	Vector3 vForward = MFD_Normalize(-m_pGameObject->GetForward());
	Vector3 vRight = MFD_Cross(m_pCamera->GetUp(), vForward);
	if(MathHelper::IsZeroVector(vRight) == false)
	{
		vRight = MFD_Normalize(vRight);
	}
	else
	{
		vRight = Vector3(1,0,0);
	}

	vPos = vPos + vRight * fValue;
	m_pGameObject->SetPos(vPos);
}

void DefaultCameraControl::Yaw(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vDir =  MFD_Normalize(-m_pGameObject->GetForward());
	//glm::mat4 mRot = glm::rotate(glm::mat4(1.0f),fValue,m_vUP);//might need this for flying game
	Matrix44 mRot = glm::rotate(fValue,m_pCamera->GetUp());//always up
	Vector3 newDir = Vector3(mRot * Vector4(vDir,0.0f));
	SetDirection(newDir.x,newDir.y,newDir.z);
}

void DefaultCameraControl::Roll(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vForward = MFD_Normalize(-m_pGameObject->GetForward());
	Matrix44 mRot = glm::rotate(fValue, vForward);
	Quaternion qRot = glm::toQuat(mRot);
	m_pGameObject->Rotate(qRot);
}

void DefaultCameraControl::Pitch(float32 fValue)
{
	m_pGameObject->RefreshTranformMat();
	Vector3 vForward = MFD_Normalize(-m_pGameObject->GetForward());
	Vector3 vRight = MFD_Cross(m_pCamera->GetUp(), vForward);
	if(MathHelper::IsZeroVector(vRight) == false)
	{
		vRight = MFD_Normalize(vRight);
	}
	else
	{
		vRight = Vector3(1,0,0);
	}

	Matrix44 mRot = glm::rotate(fValue,vRight);//always up
	Vector3 newDir = Vector3(mRot * Vector4(vForward,0.0f));
	SetDirection(newDir.x,newDir.y,newDir.z);	
}

void 
DefaultCameraControl::SetMoveSpeed(float32 fValue)
{
	m_fMoveSpeed = fValue;
}

void 
DefaultCameraControl::SetTurnSpeed(float32 fValue)
{
	m_fTurnSpeed = fValue;
}

void 
DefaultCameraControl::SetPos(float32 fx, float32 fy, float32 fz)
{
	m_pGameObject->SetPos(fx,fy,fz);
}

void 
DefaultCameraControl::SetDirection(float32 fx, float32 fy, float32 fz)
{
	Vector3 vDirection(fx,fy,fz);
	if(MathHelper::IsZeroVector(vDirection))
	{
		vDirection.z = -1;
	}
	else
	{
		vDirection = MFD_Normalize(vDirection);
	}

	Quaternion qRot = MathHelper::RotateTo(m_vDefaultLookingDir, vDirection);
	m_pGameObject->SetOrientation(qRot);
}

void 
DefaultCameraControl::SetTarget(float32 fx, float32 fy, float32 fz)
{
	Vector3 vDirection = Vector3(fx,fy,fz) - m_pGameObject->GetWorldPos();
	if(MathHelper::IsZeroVector(vDirection))
	{
		vDirection.z = -1;
	}
	else
	{
		vDirection = MFD_Normalize(vDirection);
	}

	Quaternion qRot = MathHelper::RotateTo(m_vDefaultLookingDir,vDirection);
	m_pGameObject->SetOrientation(qRot);
}