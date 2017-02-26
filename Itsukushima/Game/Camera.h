/**
* Camera Class, just handle the project and view
*
* @author: Kai Yang
**/

#ifndef CAMERA_H
#define CAMERA_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>

//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public GameObjectComponent
{
public:
	Camera(void);
	~Camera(void);

	void SetFOV(float32 fFOV);
	void SetRatio(float32 fRatio);
	void SetNearClip(float32 fNearClip);
	void SetFarClip(float32 fFarClip);

	float32 GetNearClip();
	float32 GetFarClip();

	Vector3 GetUp();

	Vector3 GetPos();
	Vector3 GetDir();

	const Matrix44* GetViewMat();
	const Matrix44* GetProjectMat();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual const char* GetName();

	static const char* ClassName();

	void CalculateScreenToWorldRay(float32 fSX, float32 fSY, uint32 uWidth, uint32 uHeight, Vector3& vPos, Vector3 &vDir);

private:
	void RefreshViewMatrix();

private:
	//view
	Vector3 m_vUP;
	Matrix44 m_mViewMatrix;

	//project
	float32	m_fFOV;
	float32 m_fRatio;
	float32 m_fNearClip;
	float32 m_fFarClip;
	Matrix44 m_mProjectionMatrix;
};

#endif
