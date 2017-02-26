/**
* A default FPS camera control GameObjectComponent, "should" only for Camera
*
* @author: Kai Yang
**/

#ifndef DEFAULT_CAMERA_CONTROL_H
#define DEFAULT_CAMERA_CONTROL_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>

class Camera;

class DefaultCameraControl : public GameObjectComponent
{
public:
	DefaultCameraControl(void);
	virtual ~DefaultCameraControl(void);

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual const char* GetName();
	static const char* ClassName();

	virtual void ProcessInput();

	void SetMoveSpeed(float32 fValue);
	void SetTurnSpeed(float32 fValue);

	void SetPos(float32 fx, float32 fy, float32 fz);
	void SetDirection(float32 fx, float32 fy, float32 fz);
	void SetTarget(float32 fx, float32 fy, float32 fz);

	void SetCamera(Camera* pCamera);

private:
	void MoveFB(float32 fValue);//forward/backward
	void MoveUD(float32 fValue);//up/down
	void MoveLR(float32 fValue);//left/right

	void Yaw(float32 fValue);
	void Roll(float32 fValue);
	void Pitch(float32 fValue);

private:
	float32 m_fTurnSpeed;
	float32 m_fMoveSpeed;

	Camera* m_pCamera;

	Vector3 m_vDefaultLookingDir;
};

#endif