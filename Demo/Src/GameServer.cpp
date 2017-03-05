#include "Itsukushima_include.h"
#include "GameServer.h"
#include "RemotePlayer.h"


GameServer::GameServer()
{
	m_nNextID = 0;
}


GameServer::~GameServer()
{
}

void 
GameServer::PreLogicUpdate()
{

}

void 
GameServer::LogicUpdate()
{

}

void 
GameServer::PostLogicUpdate()
{

}

void 
GameServer::NetworkUpdate()
{
	PackData();
	//StartSend();
}

void 
GameServer::GraphicUpdate()
{

}


const char* 
GameServer::GetName()
{
	return "GameServer";
}

const char* 
GameServer::ClassName()
{
	return "GameServer";
}

void 
GameServer::AddNewClient(int32 nSocketFD)
{
	Scene* pScene = m_pGameObject->GetScene();

	//create a gameobject in the scene to represent the remote client
	char pszName[NAME_CBUFFER_SIZE];
	sprintf_s(&pszName[0], NAME_CBUFFER_SIZE,"Client@%d",nSocketFD);
	GameObject* pObject = pScene->CreateGameObject(pszName);

	Vector2 vMinBound(-19,-9);
	Vector2 vMaxBound(19,9);

	RemotePlayer* pRemotePlayer = new RemotePlayer();
	pObject->AddComponent(pRemotePlayer);

	if(m_Team1Players.size() <= m_Team2Players.size())
	{
		//add to team1
		m_Team1Players.push_back(pRemotePlayer);
		
		Vector3 spawnPos;
		spawnPos.y = 1.0f;
		spawnPos.z = MathHelper::RandI(-9,9);
		spawnPos.x = -16 + MathHelper::RandI(-3,3);
		pRemotePlayer->Init(++m_nNextID,spawnPos, 1, this, nullptr, nSocketFD);
	}
	else
	{
		//add to team2
		m_Team2Players.push_back(pRemotePlayer);

		Vector3 spawnPos;
		spawnPos.y = 1.0f;
		spawnPos.z = MathHelper::RandI(-9,9);
		spawnPos.x = 17 + MathHelper::RandI(-3,3);
		pRemotePlayer->Init(++m_nNextID,spawnPos, 2, this, nullptr, nSocketFD);
	}

	pRemotePlayer->SetBound(vMinBound,vMaxBound);
}

void 
GameServer::RemoveClient(int32 nSocketFD)
{
	char pszName[NAME_CBUFFER_SIZE];
	sprintf_s(&pszName[0], NAME_CBUFFER_SIZE,"Client@%d",nSocketFD);

	std::list<RemotePlayer*>::iterator it;

	RemotePlayer* pPlayer = nullptr;
	for(it = m_Team1Players.begin(); it != m_Team1Players.end(); ++it)
	{
		pPlayer = (*it);
		if(pPlayer->GetSocketFD() == nSocketFD)
		{
			pPlayer->Disconnect();
			m_Team1Players.remove(pPlayer);
			return;
		}
	}

	for(it = m_Team2Players.begin(); it != m_Team2Players.end(); ++it)
	{
		pPlayer = (*it);
		if(pPlayer->GetSocketFD() == nSocketFD)
		{
			pPlayer->Disconnect();
			m_Team2Players.remove(pPlayer);
			return;
		}
	}
}

void 
GameServer::ReceiveData(NS_DataPacket* pDataPacket)
{
	NS_Base* pBase = (NS_Base*)&pDataPacket->data[0];

	if(pBase->targetID < 0)
	{
		ProcessData(pBase);
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
GameServer::ProcessData(NS_Base* pData)
{
	Debug::Assert(false, "so far nothing need to process here");
}