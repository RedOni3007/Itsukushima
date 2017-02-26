#include <Game/GameObjectComponent.h>


GameObjectComponent::GameObjectComponent()
{
	m_pGameObject = nullptr;
	m_bActive = true;
}


GameObjectComponent::~GameObjectComponent()
{
	m_pGameObject = nullptr;
}

void 
GameObjectComponent::PreLogicUpdate()
{
	//do nothing
}

void 
GameObjectComponent::LogicUpdate()
{
	//do nothing
}

void 
GameObjectComponent::PostLogicUpdate()
{
	//do nothing
}

void GameObjectComponent::NetworkUpdate()
{
	//do nothing
}

void 
GameObjectComponent::GraphicUpdate()
{
	//do nothing
}

void 
GameObjectComponent::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	//do nothing
}

void 
GameObjectComponent::Enable(bool bEnable)
{
	m_bActive = bEnable;
}

bool 
GameObjectComponent::IsEnabled()
{
	return m_bActive;
}

const char* 
GameObjectComponent::ClassName()
{
	return "GameObjectComponent";
}

GameObject* 
GameObjectComponent::GetGameObject()
{
	return m_pGameObject;
}