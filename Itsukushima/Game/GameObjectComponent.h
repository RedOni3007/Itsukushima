/**
* GameObjectComponent master class
* The GameObjectComponent should not inlcude location information, and only use its parent GameObject's loction
*
* @author: Kai Yang
**/

#ifndef GAME_OBJECT_COMPONENT
#define GAME_OBJECT_COMPONENT

#include <Game/GameObject.h>
#include <Core/RefCount.h>


class GameObjectComponent : public RefCountBase
{
	friend class GameObject;

public:
	GameObjectComponent();
	virtual ~GameObjectComponent();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual void NetworkUpdate();//a special update, which only is called from NetworkManager

	virtual const char* GetName() = 0;//todo:create a fast compareable guid
	static const char* ClassName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

	void Enable(bool bEnable);
	bool IsEnabled();

	GameObject* GetGameObject();

protected:
	GameObject* m_pGameObject;
	bool m_bActive; 

};

#endif
