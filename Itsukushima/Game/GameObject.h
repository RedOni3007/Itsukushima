/**
* GameObject Class 
* Just like a layer of tranforms matrix if it's empty
* todo:might change back to Euler xyz rotation major from the Quaternion orientation, it's much easier to put limit on Euler xyz
*
* @author: Kai Yang
**/


#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <Core/CoreHeaders.h>
#include <vector>
#include <list>

class Scene;
class GameObjectComponent;
class Model;
class Collider;
class RigidBody;
class ServerControl;
class ClientControl;
struct CollisionInfo;

class GameObject : public RefCountBase
{
friend class GameObjectComponent;
friend class Scene;

public:
	GameObject(const char* pszName);
	virtual ~GameObject();

	virtual void PreLogicUpdate();//gerenal for reset values, RefreshTranformMat will not be called
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();

	virtual void NetworkUpdate();//a special update, which only is called from NetworkManager

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

	bool AddComponent(GameObjectComponent* pComponent);
	GameObjectComponent* GetComponent(const char* pszName);

	const char* GetName();

	Matrix44* GetTranformMat();

	Quaternion GetOrientation();
	Vector3 GetScale();

	Vector3 GetWorldPos();//should use this one most of the time

	Vector3 GetWorldRight();
	Vector3 GetWorldUp();
	Vector3 GetWorldForward();

	//local axis
	Vector3 GetRight();
	Vector3 GetUp();
	Vector3 GetForward();


	//remember to call RefreshTranformMat, if need an updated world matrix right after called these set functions 
	void SetPos(Vector3 vPos);

	void SetRot(Vector3 vRot);
	void SetRot(Vector3 vAxis, float32 fAngle);
	void SetScale(Vector3 vScale);

	void SetPos(float32 x, float32 y, float32 z);
	void SetScale(float32 x, float32 y, float32 z);

	void SetOrientation(Quaternion& qOrientation);

	Model* CreateModelComponent(const char* pszMeshName);
	Collider* CreateColliderComponent(const char* pszColliderMeshName);
	Collider* CreateColliderComponent(float32 fRadius);
	RigidBody* CreateRigidBodyComponent();

	void RefreshTranformMat();

	void MovePos(Vector3& vMovement);
	void Rotate(Quaternion& qRot);

	bool IsMoved();

	void SetParent(GameObject* pParent);
	GameObject* GetParent();

	Scene* GetScene();

	//all based on local axis, RefreshTranformMat will be called in Yaw/Roll/Pitch
	void MoveFB(float32 fValue);//forward/backward
	void MoveUD(float32 fValue);//up/down
	void MoveLR(float32 fValue);//left/right
	void Yaw(float32 fValue);
	void Roll(float32 fValue);
	void Pitch(float32 fValue);

	void Destroy(float32 fDelay = -1.0f);

	bool IsDead();

private:
	Matrix44* GetParentMat();

	Vector3* GetPos();

private:
	char *m_pszName;
	
	std::list<GameObjectComponent*> m_componentList;

	Vector3		m_vPos;
	Vector3		m_vWorldPos;//when has parent
	Vector3		m_vLastPos;

	Vector3		m_vScale;
	Vector3		m_vLastScale;

	Quaternion  m_qOrientation;
	Quaternion  m_qLastOrientation;


	Matrix44 m_mRotMat;
	Matrix44 m_mTranMat;
	Matrix44 m_mScaleMat;
	Matrix44 m_mLocalWorldMat;
	Matrix44 m_mWorldMat;//final matrix

	GameObject* m_pParent;

	Scene* m_pScene;//the scene owns this object

	bool m_bDead;
	float32 m_fDeathCD;

//quick ref
public:
	Model*			GetModelComponent();
	Collider*		GetColliderComponent();
	RigidBody*		GetRigidBodyComponent();
	ServerControl*	GetServerControlComponent();
	ClientControl*	GetClientControlComponent();

	void		SetModelComponent(Model* pMode);
	void		SetColliderComponent(Collider* pCollider);
	void		SetRigidBodyComponent(RigidBody* pRigidBody);
	void		SetServerControlComponent(ServerControl* pServerControl);
	void		SetClientControlComponent(ClientControl* pClientControl);

private:
	Model*		m_pModel;
	Collider*	m_pCollider;
	RigidBody*	m_pRigidBody;
	ServerControl* m_pServerControl;
	ClientControl* m_pClientControl;

	bool m_bMoved;
	uint32 m_uMovedCount;
};


#endif