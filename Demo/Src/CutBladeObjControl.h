/*
* Used to control the master object of cutting blade(the sphere object)
*
* @author: Kai Yang
*/

#ifndef CUT_BLADE_OBJECT_CONTROL_H
#define CUT_BLADE_OBJECT_CONTROL_H


class Camera;

class CutBladeObjControl : public GameObjectComponent
{
public:
	CutBladeObjControl();
	virtual ~CutBladeObjControl();

	static const char* ClassName();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

	void SetCamera(Camera* pCamera);


private:
	void ProcessInput();

	void MoveFB(float32 fValue);
	void MoveLR(float32 fValue);
	void TurnLR(float32 fValue);

private:
	Camera* m_pCamera;

	float32 m_fTurnSpeed;
	float32 m_fMoveSpeed;

};

#endif