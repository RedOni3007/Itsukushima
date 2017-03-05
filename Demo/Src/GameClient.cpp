#include "Itsukushima_include.h"
#include "GameClient.h"
#include "NetworkStruct.h"
#include "RemotePlayer.h"
#include "Basement.h"

GameClient::GameClient(void)
{
	m_nServerSideID = -1;
	m_pRemoteIO = nullptr;
	m_Basement1 = nullptr;
	m_Basement2 = nullptr;
}


GameClient::~GameClient(void)
{
	if(m_pRemoteIO != nullptr)
	{
		delete m_pRemoteIO;
		m_pRemoteIO = nullptr;
	}

	m_Basement1 = nullptr;
	m_Basement2 = nullptr;
}

void 
GameClient::PreLogicUpdate()
{

}

void 
GameClient::LogicUpdate()
{

}

void 
GameClient::PostLogicUpdate()
{

}

void 
GameClient::NetworkUpdate()
{
	if(m_pRemoteIO == nullptr)
	{
		m_pRemoteIO = new RemoteIO(-1);
	}

	ProcessNetwrokData();
	PackData();
	//StartSend();
}

void 
GameClient::GraphicUpdate()
{

}

const char* 
GameClient::GetName()
{
	return "GameClient";
}

const char* 
GameClient::ClassName()
{
	return "GameClient";
}

void 
GameClient::AddNewClient(int32 nSocketFD)
{

}

void 
GameClient::RemoveClient(int32 nSocketFD)
{

}

void 
GameClient::ProcessNetwrokData()
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
GameClient::ReceiveData(NS_DataPacket* pDataPacket)
{
	if(m_pRemoteIO == nullptr)
	{
		m_pRemoteIO = new RemoteIO(-1);
	}

	NS_Base* pBase = (NS_Base*)&pDataPacket->data[0];

	if(pBase->targetID < 0)
	{
		m_pRemoteIO->AddReceivedData(pDataPacket);
	}
	else
	{
		std::list<RemotePlayer*>::iterator it;
		RemotePlayer* pPlayer = nullptr;
		for(it = m_Team1Players.begin(); it != m_Team1Players.end(); ++it)
		{
			pPlayer = (*it);
			if(pPlayer->GetID() == pBase->targetID)
			{
				pPlayer->ReceiveData(pDataPacket);
				return;
			}
		}

		for(it = m_Team2Players.begin(); it != m_Team2Players.end(); ++it)
		{
			pPlayer = (*it);
			if(pPlayer->GetID() == pBase->targetID)
			{
				pPlayer->ReceiveData(pDataPacket);
				return;
			}
		}

	}


}

void 
GameClient::ProcessData(NS_Base* pData)
{
	if(pData->nType == (int32)NetWorkStructEnum::ASSIGN_ID)
	{
		m_nServerSideID = ((NS_AssignID*)pData)->id;
	}
	else if(pData->nType == (int32)NetWorkStructEnum::PLAYER_JOIN)
	{
		NS_PlayerJoin* pNS_PlayerJoin = (NS_PlayerJoin*)pData;

		char pszName[NAME_CBUFFER_SIZE];
		sprintf_s(&pszName[0], NAME_CBUFFER_SIZE,"Client@%d",pNS_PlayerJoin->id);

		Scene* pScene = m_pGameObject->GetScene();
		GameObject* pObject = pScene->CreateGameObject(pszName);

		RemotePlayer* pRemotePlayer = new RemotePlayer();
		pObject->AddComponent(pRemotePlayer);

			//add to team1
		if(pNS_PlayerJoin->team == 1)
		{
			m_Team1Players.push_back(pRemotePlayer);
		}
		else
		{
			m_Team2Players.push_back(pRemotePlayer);
		}
		
		bool bLocalPlayer = (pNS_PlayerJoin->id == m_nServerSideID);
		pRemotePlayer->Init(pNS_PlayerJoin->id,pNS_PlayerJoin->spawnPos, pNS_PlayerJoin->team, nullptr, this, -1, bLocalPlayer);

		Vector2 vMinBound(-19,-9);
		Vector2 vMaxBound(19,9);
		pRemotePlayer->SetBound(vMinBound,vMaxBound);
		
	}
	else if(pData->nType == (int32)NetWorkStructEnum::PLAYER_LEAVE)
	{
		NS_PlayerLeave* pNS_PlayerLeave = (NS_PlayerLeave*)pData;
		std::list<RemotePlayer*>::iterator it;
		RemotePlayer* pPlayer = nullptr;
		for(it = m_Team1Players.begin(); it != m_Team1Players.end(); ++it)
		{
			pPlayer = (*it);
			if(pPlayer->GetID() == pNS_PlayerLeave->id)
			{
				pPlayer->Disconnect();
				return;
			}
		}

		for(it = m_Team2Players.begin(); it != m_Team2Players.end(); ++it)
		{
			pPlayer = (*it);
			if(pPlayer->GetID() == pNS_PlayerLeave->id)
			{
				pPlayer->Disconnect();
				return;
			}
		}
	}
	else if(pData->nType == (int32)NetWorkStructEnum::BASEMENTS_STATUS)
	{
		NS_BasementStatus* pNS_BasementStatus = (NS_BasementStatus*)pData;

		if(m_Basement1 == nullptr)
		{
			GameObject* pObject = m_pGameObject->GetScene()->GetGameObject("Base_Team1");
			if(pObject != nullptr)
			{
				m_Basement1 = (Basement*)pObject->GetComponent(Basement::ClassName());
				Debug::Assert(m_Basement1 != nullptr);
			}
		}

		if(m_Basement2 == nullptr)
		{
			GameObject* pObject = m_pGameObject->GetScene()->GetGameObject("Base_Team2");
			if(pObject != nullptr)
			{
				m_Basement2 = (Basement*)pObject->GetComponent(Basement::ClassName());
				Debug::Assert(m_Basement2 != nullptr);
			}
		}

		if(pNS_BasementStatus->nTeamID == 1)
		{
			m_Basement1->SetHealth(pNS_BasementStatus->nHealth);
		}
		else if(pNS_BasementStatus->nTeamID == 2)
		{
			m_Basement2->SetHealth(pNS_BasementStatus->nHealth);
		}

	}
	else if(pData->nType == (int32)NetWorkStructEnum::OTHER_PlAYER_INIT)
	{
		NS_OtherPlayerInit* pNS_OtherPlayerInit = (NS_OtherPlayerInit*)pData;

		char pszName[NAME_CBUFFER_SIZE];
		sprintf_s(&pszName[0], NAME_CBUFFER_SIZE,"Client@%d",pNS_OtherPlayerInit->id);

		Scene* pScene = m_pGameObject->GetScene();
		GameObject* pObject = pScene->CreateGameObject(pszName);

		RemotePlayer* pRemotePlayer = new RemotePlayer();
		pObject->AddComponent(pRemotePlayer);

			//add to team1
		if(pNS_OtherPlayerInit->team == 1)
		{
			m_Team1Players.push_back(pRemotePlayer);
		}
		else
		{
			m_Team2Players.push_back(pRemotePlayer);
		}
		
		pRemotePlayer->Init(pNS_OtherPlayerInit->id,pNS_OtherPlayerInit->spawnPos, pNS_OtherPlayerInit->team, nullptr, this, -1, false);

		Vector2 vMinBound(-19,-9);
		Vector2 vMaxBound(19,9);
		pRemotePlayer->SetBound(vMinBound,vMaxBound);

		m_pGameObject->SetPos(pNS_OtherPlayerInit->vPos);
		m_pGameObject->SetOrientation(pNS_OtherPlayerInit->qRot);
		pRemotePlayer->SetHealth(pNS_OtherPlayerInit->nHealth);
	}
	else
	{
		Debug::Assert(false, "unknown type");
	}
}