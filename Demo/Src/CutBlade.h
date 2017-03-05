/*
* The cutting blade control to handle the swing and objects destroy/create.
*
* @author: Kai Yang
*/
#ifndef CUT_BLADE_H
#define CUT_BLADE_H

#include <vector>
#include <list>

struct PlaneHitInfo;

class CutBlade : public GameObjectComponent
{
public:
	CutBlade();
	virtual ~CutBlade();
	
	static const char* ClassName();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual const char* GetName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

	void Init();

	void SetHolderObecjt(GameObject* pHolder);
	void SetHolderObecjt_Yaw(GameObject* pHolder);
	void SetHolderObecjt_Roll(GameObject* pHolder);

	void SwitchCutMethod();

private:
	void ProcessInput();

	void StartSwing();
	void EndSwing();

	bool CutObject(GameObject* pObject);

private:
	GameObject* m_pHolderObject;
	GameObject* m_pHolderObject_Yaw;
	GameObject* m_pHolderObject_Roll;

	Quaternion m_qOriginOrientation;

	float32 m_fSwingTime;
	float32 m_fSwingCD;
	float32 m_fSwingSpeed;
	bool m_bInSwing;

	float32 m_fRollSpeed;
	float32 m_fYawSpeed;

	bool m_bComplexCut;

	float32 m_fObjectStayTime;

	std::vector<GameObject*> m_touchedObjects;
	std::vector<GameObject*> m_newObjects;

	//temp limit, should be done in GameObject class
	float32 m_fRoll;
	float32 m_fYaw;

	PlaneHitInfo* m_pHitInfo;
};

#endif