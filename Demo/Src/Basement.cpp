#include "Itsukushima_include.h"
#include "Basement.h"
#include "NetworkStruct.h"
#include "Parser_NetworkStruct.h"
#include "NetworkStruct.h"
#include "RemotePlayer.h"
#include "GameServer.h"

#define MAX_HEALTH 500

Basement::Basement(void)
{
	m_nTeamNum = -1;

	m_pServer = nullptr;

	m_nHealth = MAX_HEALTH;

	m_bSpawning = false;
	m_fSpawningCD = 0.0f;
}


Basement::~Basement(void)
{
	m_pPlaceholder = nullptr;
	m_pServer = nullptr;
}


int32 
Basement::GetTeamID()
{
	return m_nTeamNum;
}

void 
Basement::PreLogicUpdate()
{
	if(m_nHealth == 0)
		Respawn();

}

void 
Basement::LogicUpdate()
{

	float fDeltaTime = Timer::Instance()->GetDeltaTime();
	
	if(m_bSpawning)
	{
		m_fSpawningCD -= fDeltaTime;
		float32 fScale = 1.0f - m_fSpawningCD;
		if(m_fSpawningCD <= 0.0f)
		{
			m_fSpawningCD = 0.0f;
			fScale = 1.0f;
			m_bSpawning = false;
		}

		m_pGameObject->SetScale(fScale,fScale,fScale);
	}
	else
	{
		float32 fheathMark = 0.1f + m_nHealth / (float32)MAX_HEALTH * 0.8f;
		m_pPlaceholder->GetModelComponent()->SetTransparency(fheathMark);	
	}
}

void 
Basement::PostLogicUpdate()
{
	if(m_pServer != nullptr)
	{
		NS_BasementStatus sNS_BasementStatus;
		sNS_BasementStatus.targetID = -1;
		sNS_BasementStatus.nTeamID = m_nTeamNum;
		sNS_BasementStatus.nHealth = m_nHealth;

		NS_DataPacket* pDataPark = new NS_DataPacket();//leave to GameServer to delete it
		NetworkStructParser::ToDataPacket(&sNS_BasementStatus, pDataPark);
		m_pServer->AddGlobalData(pDataPark);
	}
}

void 
Basement::GraphicUpdate()
{

}


const char* 
Basement::GetName()
{
	return "Basement";
}

const char* 
Basement::ClassName()
{
	return "Basement";
}


void 
Basement::Init(Vector3 vPos, int32 nTeamNum, GameServer* pServer)
{
	m_nTeamNum = nTeamNum;
	m_pServer = pServer;

	m_pGameObject->SetPos(vPos);
	m_pGameObject->CreateColliderComponent(2.0f);

	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	RigidBody* pRigidBody = nullptr;

	pObject = m_pGameObject->GetScene()->CreateGameObject("Base_Placeholder");
	pObject->SetPos(Vector3(0.0f,0.0f,0.0f));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(2,2,2));
	pModel = pObject->CreateModelComponent("SPHERE");
	pModel->SetMaterial("Cross");
	pModel->SetRenderPass(RenderPass::GBUFFER,false);
	pModel->SetRenderPass(RenderPass::TRANSPARENT,true);
	pRigidBody = pObject->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	if(nTeamNum == 1)
	{
		pModel->SetTintColor(Vector4(1,0.5f,0.5f,1));
		pRigidBody->SetAngularVelocity(Vector3(0,1,0));
	}
	else
	{
		pModel->SetTintColor(Vector4(0.5f,0.5f,1,1));
		pRigidBody->SetAngularVelocity(Vector3(0,-1,0));
	}
	pModel->SetTransparency(0.9f);

	pObject->SetParent(m_pGameObject);
	m_pPlaceholder = pObject;
}


void 
Basement::Respawn()
{
	m_bSpawning = true;
	m_fSpawningCD = 1.0f;

	m_nHealth = MAX_HEALTH;
}



void 
Basement::ProcessData(NS_Base* pData)
{

}

void 
Basement::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	if(pCollidedObj != nullptr)
	{
		RemotePlayer* pPlayer = nullptr;

		if(pCollidedObj->GetParent() != nullptr)
		{
			pPlayer = (RemotePlayer*)pCollidedObj->GetParent()->GetComponent(RemotePlayer::ClassName());
		}

		if(pPlayer == nullptr)
		{
			//hit something I do not care
			return;
		}

		if(pPlayer->GetTeamID() != m_nTeamNum)
		{
			TakeDamage(1);
		}
	}
}



void 
Basement::TakeDamage(int32 nDamage)
{
	if(m_pServer == nullptr)
		return;

	if(m_bSpawning)
		return;

	m_nHealth -= nDamage;
	if(m_nHealth < 0)
		m_nHealth = 0;
}

void 
Basement::SetHealth(int nHealth)
{
	m_nHealth = nHealth;
}
