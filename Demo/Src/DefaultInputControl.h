/*
* Used to control a object by build-in arrow key define
*
* @author: Kai Yang
*/

#ifndef DEFAULT_INPUT_CONTROL_H
#define DEFAULT_INPUT_CONTROL_H

class DefaultInputControl  : public GameObjectComponent
{
public:
	DefaultInputControl();
	virtual ~DefaultInputControl();

	static const char* ClassName();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);


private:
	void ProcessInput();

	void MoveFB(float32 fValue);
	void MoveLR(float32 fValue);
	void TurnLR(float32 fValue);

private:
	float32 m_fTurnSpeed;
	float32 m_fMoveSpeed;

};

#endif