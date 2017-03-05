#include "Itsukushima_include.h"
#include "RemotePlayer.h"
#include "NetworkStruct.h"
#include "Parser_NetworkStruct.h"
#include "NetworkStruct.h"
#include "DefaultInputControl.h"
#include "GameServer.h"
#include "GameClient.h"

#define MAX_HEALTH 100

RemotePlayer::RemotePlayer(void)
{
	m_nID = -1;
	m_nSocketFD = -1;
	m_nTeamNum = -1;
	m_bAlive = false;
	m_fSpawnCD = 0;

	m_pGameServer = nullptr;
	m_pGameclient = nullptr;

	m_pAvatarBody = nullptr;
	m_pDirectionMark = nullptr;
	m_pAttackArm1 = nullptr;
	m_pAttackArm2 = nullptr;
	m_pPlayerMark = nullptr;

	m_bGoingToInit = false;
	m_bInited = false;

	m_pRemoteIO = nullptr;

	m_bOnServer = false;

	m_fMoveSpeed = 5.0f;

	m_bAttacking = false;
	m_fAttackCD = 0.0f;

	m_bSpawning = false;
	m_fSpawningCD = 0.0f;

	m_nHealth = MAX_HEALTH;
}


RemotePlayer::~RemotePlayer(void)
{
	m_pGameServer = nullptr;
	m_pGameclient = nullptr;

	m_pAvatarBody = nullptr;
	m_pDirectionMark = nullptr;
	m_pAttackArm1 = nullptr;
	m_pAttackArm2 = nullptr;
	m_pPlayerMark = nullptr;
	m_pRemoteIO = nullptr;
}

int32 
RemotePlayer::GetSocketFD()
{
	return m_nSocketFD;
}

int32
RemotePlayer::GetID()
{
	return m_nID;
}

int32 
RemotePlayer::GetTeamID()
{
	return m_nTeamNum;
}

RemoteIO* 
RemotePlayer::GetRemoteIO()
{
	return m_pRemoteIO;
}

void 
RemotePlayer::PreLogicUpdate()
{
	if(m_bGoingToInit)
	{
		//something need to be done on main thread
		m_pGameObject->SetPos(m_vSpawnPos);

		char pszMaterialName[NAME_CBUFFER_SIZE];
		sprintf_s(&pszMaterialName[0], NAME_CBUFFER_SIZE,"Team%d",m_nTeamNum);

		GameObject* pObject = nullptr;
		Model* pModel = nullptr;
		RigidBody* pRigidBody = nullptr;

		m_pGameObject->CreateColliderComponent("Box_Collision");//hit box

		pObject = m_pGameObject->GetScene()->CreateGameObject("PlayerAvatar");
		pObject->SetPos(Vector3(0.0f,0.0f,0.0f));
		pObject->SetRot(Vector3(0,0,0));
		pObject->SetScale(Vector3(1,1,1));
		pModel = pObject->CreateModelComponent("CUBE_LOW");
		pModel->SetMaterial(pszMaterialName);
		pObject->SetParent(m_pGameObject);
		m_pAvatarBody = pObject;

		pObject = m_pGameObject->GetScene()->CreateGameObject("Attack Arm1");
		pObject->SetPos(Vector3(1.05f, 0.0f, 0.0f));
		pObject->SetRot(Vector3(0,0,0));
		pObject->SetScale(Vector3(1.0f,1.1f,0.1f));
		pModel = pObject->CreateModelComponent("CUBE_LOW");
		pModel->SetMaterial(pszMaterialName);
		pObject->CreateColliderComponent("Box_Collision");
		pObject->SetParent(m_pGameObject);
		m_pAttackArm1 = pObject;

		pObject = m_pGameObject->GetScene()->CreateGameObject("Attack Arm2");
		pObject->SetPos(Vector3(-1.05f, 0.0f, 0.0f));
		pObject->SetRot(Vector3(0,0,0));
		pObject->SetScale(Vector3(1.0f,1.1f,0.1f));
		pModel = pObject->CreateModelComponent("CUBE_LOW");
		pModel->SetMaterial(pszMaterialName);
		pObject->CreateColliderComponent("Box_Collision");
		pObject->SetParent(m_pGameObject);
		m_pAttackArm2 = pObject;

		//direction mark
		pObject = m_pGameObject->GetScene()->CreateGameObject("DirMark");
		pObject->SetPos(Vector3(0.0f,0.0f,0.75f));
		pObject->SetRot(Vector3(90,0,0));
		pObject->SetScale(Vector3(1,1,1));
		pModel = pObject->CreateModelComponent("D4");
		pModel->SetMaterial(pszMaterialName);
		pRigidBody = pObject->CreateRigidBodyComponent();
		pRigidBody->SetStatic(true);
		pRigidBody->SetAngularVelocity(Vector3(0,0,6));
		pObject->SetParent(m_pGameObject);
		m_pDirectionMark = pObject;

		m_bInited = true;
		m_bGoingToInit = false;

		if(m_bIsLocalPlayer)
		{
			//add local player mark
			pObject = m_pGameObject->GetScene()->CreateGameObject("PlayerMark");
			pObject->SetPos(Vector3(0.0f,1.0f,0.0f));
			pObject->SetRot(Vector3(180,0,0));
			pObject->SetScale(Vector3(1,1,1));
			pModel = pObject->CreateModelComponent("D4");
			pModel->SetMaterial("Solid");
			pRigidBody = pObject->CreateRigidBodyComponent();
			pRigidBody->SetStatic(true);
			pRigidBody->SetAngularVelocity(Vector3(0,3,0));
			pObject->SetParent(m_pGameObject);
			m_pPlayerMark = pObject;


			//add input control to local player			
			DefaultInputControl* pDefaultInputControl = new DefaultInputControl();
			m_pGameObject->AddComponent(pDefaultInputControl);
		}
	}

	if(m_bAlive == false && m_bInited)
	{
		float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
		m_fSpawnCD -= fDeltaTime;
		if(m_fSpawnCD < 0)
		{
			Respawn();
		}
	}
	else if(m_nHealth == 0)
	{
		Respawn();
	}
}

void 
RemotePlayer::LogicUpdate()
{
	ProcessNetwrokData();

	float fDeltaTime = Timer::Instance()->GetDeltaTime();
	if(m_bSpawning == false)
	{
		ProcessInput();

		if(GlobalSetting::bTrueGame)
		{
			m_pGameObject->MoveFB(m_fMoveSpeed * fDeltaTime);
		}

	}

	if(m_bAttacking && m_fAttackCD < 1.0f)
	{
		m_fAttackCD += fDeltaTime;
		if(m_fAttackCD >= 1.0f)
		{
			m_pAttackArm1->SetRot(Vector3(0,0,0));
			m_pAttackArm2->SetRot(Vector3(0,0,0));
			m_bAttacking = false;
		}
		else
		{
			Quaternion qRot = Quaternion(Vector3(0.6f,0,0));
			m_pAttackArm1->Rotate(qRot);
			m_pAttackArm2->Rotate(qRot);
		}
	}
	
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
		float32 fheathMark = 0.3f + m_nHealth / (float32)MAX_HEALTH * 0.7f;
		Vector4 vTintColor(fheathMark,fheathMark,fheathMark,1.0f);
		m_pAvatarBody->GetModelComponent()->SetTintColor(vTintColor);
		m_pDirectionMark->GetModelComponent()->SetTintColor(vTintColor);
		m_pAttackArm1->GetModelComponent()->SetTintColor(vTintColor);
		m_pAttackArm2->GetModelComponent()->SetTintColor(vTintColor);
	}
}

void 
RemotePlayer::PostLogicUpdate()
{
	float fDeltaTime = Timer::Instance()->GetDeltaTime();

	//apply bound
	float32 fTurnSpeed = 100;
	Vector3 vPos = m_pGameObject->GetWorldPos();
	Vector3 vForward = m_pGameObject->GetWorldForward();
	if(vPos.x < m_vMinBound.x)
	{
		vPos.x = m_vMinBound.x;

		float32 fTurn = 1.0f * fDeltaTime * fTurnSpeed;
		if(vForward.z < 0)
			fTurn *= -1;

		m_pGameObject->SetPos(vPos);
		m_pGameObject->Yaw(fTurn);
	}
	else if(vPos.x > m_vMaxBound.x)
	{
		vPos.x = m_vMaxBound.x;

		float32 fTurn = 1.0f * fDeltaTime * fTurnSpeed;
		if(vForward.z > 0)
			fTurn *= -1;

		m_pGameObject->SetPos(vPos);
		m_pGameObject->Yaw(fTurn);
	}

	if(vPos.z < m_vMinBound.y)
	{
		vPos.z = m_vMinBound.y;

		float32 fTurn = 1.0f * fDeltaTime * fTurnSpeed;
		if(vForward.x > 0)
			fTurn *= -1;

		m_pGameObject->SetPos(vPos);
		m_pGameObject->Yaw(fTurn);
	}
	else if(vPos.z > m_vMaxBound.y)
	{
		vPos.z = m_vMaxBound.y;

		float32 fTurn = 1.0f * fDeltaTime * fTurnSpeed;
		if(vForward.x < 0)
			fTurn *= -1;

		m_pGameObject->SetPos(vPos);
		m_pGameObject->Yaw(fTurn);
	}

	if(m_bIsLocalPlayer || m_bOnServer)
	{
		NS_PlayerStatus sPlayerStatus;
		sPlayerStatus.targetID = m_nID;
		sPlayerStatus.vPos = m_pGameObject->GetWorldPos();
		sPlayerStatus.qRot = m_pGameObject->GetOrientation();
		sPlayerStatus.nHealth = m_nHealth;

		NS_DataPacket* pDataPark = new NS_DataPacket();//leave to GameServer to delete it
		NetworkStructParser::ToDataPacket(&sPlayerStatus, pDataPark);

		if(m_bOnServer)
			m_pGameServer->AddGlobalData(pDataPark);//tell everyone

		if(m_bIsLocalPlayer)
			m_pGameclient->AddGlobalData(pDataPark);//tell server
	}
}

void 
RemotePlayer::GraphicUpdate()
{

}


const char* 
RemotePlayer::GetName()
{
	return "RemotePlayer";
}

const char* 
RemotePlayer::ClassName()
{
	return "RemotePlayer";
}


void 
RemotePlayer::Init(int32 nID, Vector3 vSpawnPos, int32 nTeamNum, GameServer* pGameServer,  GameClient*  pGameclient,int32 nSocketFD,  bool bIsLocalPlayer)
{
	if(m_bGoingToInit || m_bInited)
	{
		return;
	}

	m_pGameServer = pGameServer;
	m_pGameclient = pGameclient;

	m_nID = nID;
	m_nSocketFD = nSocketFD;
	m_vSpawnPos =vSpawnPos;
	m_nTeamNum = nTeamNum;
	m_bGoingToInit = true;

	m_bIsLocalPlayer = bIsLocalPlayer;
	if(m_pGameServer != nullptr)
	{
		//this object is on the server side
		m_bOnServer = true;
	}

	if(m_pRemoteIO == nullptr)
	{
		m_pRemoteIO = new RemoteIO(nSocketFD);
		m_pGameObject->AddComponent(m_pRemoteIO);
	}

	if(m_bOnServer)
	{
		//self id + other players info
		std::list<void*> otherPlayers;
		m_pGameObject->GetScene()->GetComponents(RemotePlayer::ClassName(),otherPlayers);

		int32 nTotalPackets = otherPlayers.size();// -self
		if(nTotalPackets <= 0)
			nTotalPackets = 1;//at least one for the ID

		NS_DataPacket* pDataPackets = new NS_DataPacket[nTotalPackets];

		NS_AssignID sAssignID;
		sAssignID.targetID = -1;
		sAssignID.id = m_nID;
		NetworkStructParser::ToDataPacket(&sAssignID, &pDataPackets[0]);

		NS_OtherPlayerInit sNS_OtherPlayerInit;
		sNS_OtherPlayerInit.targetID = -1;
		RemotePlayer* pPlayer = nullptr;
		int index = 1;
		std::list<void*>::iterator it;
		for(it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
		{
			pPlayer = (RemotePlayer*)(*it);
			if(pPlayer->GetID() == m_nID)
				continue;

			sNS_OtherPlayerInit.id = pPlayer->GetID();
			sNS_OtherPlayerInit.team = pPlayer->GetTeamID();
			sNS_OtherPlayerInit.spawnPos = pPlayer->m_vSpawnPos;
			sNS_OtherPlayerInit.vPos = pPlayer->m_pGameObject->GetWorldPos();
			sNS_OtherPlayerInit.qRot = pPlayer->m_pGameObject->GetOrientation();
			sNS_OtherPlayerInit.nHealth = pPlayer->m_nHealth;

			NetworkStructParser::ToDataPacket(&sNS_OtherPlayerInit, &pDataPackets[index]);
			++index;
		}
		m_pRemoteIO->AddDataOnlyToThisIO(pDataPackets,nTotalPackets);
		delete[] pDataPackets;

		//join info
		NS_PlayerJoin sNetworkStruct;
		sNetworkStruct.targetID = -1;
		sNetworkStruct.id = m_nID;
		sNetworkStruct.team = nTeamNum;
		sNetworkStruct.spawnPos = vSpawnPos;

		NS_DataPacket* pDataPark = new NS_DataPacket();//leave to GameServer to delete it
		NetworkStructParser::ToDataPacket(&sNetworkStruct, pDataPark);
		m_pGameServer->AddGlobalData(pDataPark);
	}

}

void 
RemotePlayer::ForceRespawn()
{
	Respawn();
}

void 
RemotePlayer::Respawn()
{
	m_bSpawning = true;
	m_fSpawningCD = 1.0f;

	m_pGameObject->SetPos(m_vSpawnPos);
	m_bAlive = true;
	m_nHealth = MAX_HEALTH;

	m_pAttackArm1->SetRot(Vector3(0,0,0));
	m_pAttackArm2->SetRot(Vector3(0,0,0));
	m_bAttacking = false;
		
	if(m_nTeamNum == 1)
	{
		m_pGameObject->SetRot(Vector3(0,90,0));
	}
	else
	{
		m_pGameObject->SetRot(Vector3(0,-90,0));
	}
}

void 
RemotePlayer::Disconnect()
{
	m_pGameObject->SetPos(0,99999,0);//move to far far away

	//a 5 second delay on destroy
	m_pGameObject->Destroy(5.0f);
	m_pAttackArm1->Destroy(5.0f);
	m_pAttackArm2->Destroy(5.0f);
	m_pAvatarBody->Destroy(5.0f);
	m_pDirectionMark->Destroy(5.0f);

	if(m_pPlayerMark)
	{
		m_pPlayerMark->Destroy(5.0f);
	}

	if(m_bOnServer)
	{
		NS_PlayerLeave sNetworkStruct;
		sNetworkStruct.targetID = -1;
		sNetworkStruct.id = m_nID;

		NS_DataPacket* pDataPark = new NS_DataPacket();//leave to GameServer to delete it
		NetworkStructParser::ToDataPacket(&sNetworkStruct, pDataPark);
		m_pGameServer->AddGlobalData(pDataPark);
	}
}

void 
RemotePlayer::ReceiveData(NS_DataPacket* pData)
{
	m_pRemoteIO->AddReceivedData(pData);
}

void 
RemotePlayer::ProcessNetwrokData()
{
	NS_DataPacket* pDataPacket = m_pRemoteIO->GetNextReceivedData();
	NS_Base* pBase = nullptr;

	while(pDataPacket != nullptr)
	{
		pBase = (NS_Base*)&pDataPacket->data[0];
		ProcessData(pBase);

		m_pRemoteIO->RemoveReceivedData(pDataPacket);
		pDataPacket = m_pRemoteIO->GetNextReceivedData();
	}
}

void 
RemotePlayer::ProcessData(NS_Base* pData)
{
	if(pData->nType == (int32)NetWorkStructEnum::PLAYER_STATUS)
	{
		NS_PlayerStatus* pNS_PlayerStatus = (NS_PlayerStatus*)pData;

		m_nHealth = pNS_PlayerStatus->nHealth;//only take damage from server

		if(m_bIsLocalPlayer)
		{
			return;
		}

		m_pGameObject->SetPos(pNS_PlayerStatus->vPos);
		m_pGameObject->SetOrientation(pNS_PlayerStatus->qRot);
	}
	else if(pData->nType == (int32)NetWorkStructEnum::PLAYER_ATTACK)
	{
		if(m_bIsLocalPlayer)
			return;

		//NS_PlayerAttack* pNS_PlayerAttack = (NS_PlayerAttack*)pData;
		Attack();
	}
	else
	{
		Debug::Assert(false, "unknown type");
	}


}

void 
RemotePlayer::OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo)
{
	if(pCollidedObj != nullptr)
	{
		RemotePlayer* pPlayer = (RemotePlayer*)pCollidedObj->GetComponent(RemotePlayer::ClassName());

		if(pPlayer == nullptr && pCollidedObj->GetParent() != nullptr)
		{
			pPlayer = (RemotePlayer*)pCollidedObj->GetParent()->GetComponent(RemotePlayer::ClassName());
		}

		if(pPlayer == nullptr)
		{
			//hit something I do not care
			return;
		}

		if(pPlayer->m_bSpawning == false && pPlayer->m_bAttacking && pPlayer->GetTeamID() != m_nTeamNum)
		{
			TakeDamage(3);
		}
	}
}

void 
RemotePlayer::ProcessInput()
{
	if(m_bIsLocalPlayer == false)
		return;

	VPad pad = InputManager::Instance()->GetPad(0);
	Vector2 vMouseMovement = InputManager::Instance()->GetMouseMovement();
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();

	if(pad.IsButtonDown(VPAD_BUTTON_SELECT))
	{
		Attack();
	}
}


void
RemotePlayer::Attack()
{
	if(m_bAttacking)
		return;

	m_bAttacking = true;
	m_fAttackCD = 0.0f;

	if(m_bIsLocalPlayer || m_bOnServer)
	{
		NS_PlayerAttack sPlayerAttack;
		sPlayerAttack.targetID = m_nID;
		sPlayerAttack.id = m_nID;

		NS_DataPacket* pDataPark = new NS_DataPacket();//leave to GameServer to delete it
		NetworkStructParser::ToDataPacket(&sPlayerAttack, pDataPark);

		if(m_bOnServer)
			m_pGameServer->AddGlobalData(pDataPark);//tell everyone

		if(m_bIsLocalPlayer)
			m_pGameclient->AddGlobalData(pDataPark);//tell server
	}

}

void 
RemotePlayer::TakeDamage(int32 nDamage)
{
	if(m_bOnServer == false)
		return;

	if(m_bSpawning)
		return;

	m_nHealth -= nDamage;
	if(m_nHealth < 0)
		m_nHealth = 0;
}

void 
RemotePlayer::SetHealth(int32 nHealth)
{
	m_nHealth = nHealth;
}

void 
RemotePlayer::SetBound(Vector2& vMin, Vector2& vMax)
{
	m_vMinBound = vMin;
	m_vMaxBound = vMax;
}