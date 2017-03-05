/*
* Used to create and launch a object into air
" This is for rigid body demo screen
*
* @author: Kai Yang
*/

#ifndef LAUNCHER_H
#define LAUNCHER_H


class ObjectPool;

class Launcher : public GameObjectComponent 
{
public:
	Launcher();
	virtual ~Launcher();

	virtual void LogicUpdate();
	virtual const char* GetName();

	static const char* ClassName();

	void SetObjectPool(ObjectPool* pObjectPool);

	void LaunchOjbect();

	//why there is no variable access function automaker in VS (¨s-_-)¨s©Ø©Ø  
	void SetStartVelocity(Vector3 vValue);
	void SetStartRotation(Vector3 vValue);

	void SetMass(float32 fValue);
	void SetDynamicFriction(float32 fValue);
	void SetStaticFriction(float32 fValue);
	void SetElasticity(float32 fValue);

	Vector3 GetStartVelocity();
	Vector3 GetStartRotation();

	float32 GetMass();
	float32 GetDynamicFriction();
	float32 GetStaticFriction();
	float32 GetElasticity();

private:
	float32 m_fCD;
	float32 m_fInitCD;

	Vector3 m_vStartVelocity;
	Vector3 m_vStartRot;

	float32 m_fMass;
	float32 m_fDynamicFriction;
	float32 m_fStaticFriction;
	float32 m_fElasticity;

	ObjectPool* m_pObjectPool;
};

#endif