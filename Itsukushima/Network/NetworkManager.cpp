#include <Network/NetworkManager.h>
#include "Server.h"
#include "Client.h"
#include <Game/Scene.h>
#include "SocketHeaders.h"
#include <Core/GlobalSetting.h>


NetworkManager::NetworkManager(void)
{
	m_bInited = false;
	m_pClient = nullptr;

	m_pszLocalName = nullptr;
}


NetworkManager::~NetworkManager(void)
{
	m_bInited = false;
	m_pClient = nullptr;

	if(m_pszLocalName != nullptr)
	{
		delete[] m_pszLocalName;
		m_pszLocalName = nullptr;
	}
}

NetworkManager* 
NetworkManager::Instance()
{
	static NetworkManager me;
	return &me;
}

void 
NetworkManager::Init()
{
	// start Winsock
	WSADATA wsaData;
	int32 iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) 
	{
		Debug::Log("WSAStartup failed: %d\n", iResult);
		Debug::Assert(false);
        return;
    }

}

void 
NetworkManager::Destroy()
{
	m_bGameSideDone = true;
	bool bAllServerStopped = false;
	while(bAllServerStopped == false)
	{
		bAllServerStopped = true;
		for(uint32 i = 0; i < m_ServerList.size(); ++i)
		{
			if(m_ServerList[i] == nullptr)
				continue;

			if(m_ServerList[i]->IsStopped() == false)
			{
				m_ServerList[i]->End();
				bAllServerStopped = false;
			}
			else
			{
				//only delete the server when its threads have stopped
				delete m_ServerList[i];
				m_ServerList[i] =nullptr;
			}
		}
	}
	m_ServerList.clear();


	if(m_pClient != nullptr)
	{
		m_pClient->End();
		while(m_pClient!= nullptr && m_pClient->IsStopped() == false)
		{}//wait the client to stop
		delete m_pClient;
		m_pClient = nullptr;
	}

	//end Winsock
	WSACleanup();
}

Server* 
NetworkManager::StartServer(const char* pszHost,const char* pszPort)
{
	if(m_pszLocalName == nullptr)
	{
		m_pszLocalName = new char[NAME_CBUFFER_SIZE];
		gethostname(m_pszLocalName, NAME_CBUFFER_SIZE);
	}

	Server* pServer = new Server();

	//override
	if(GlobalSetting::bOverrideHostName)
	{
		GlobalSetting::pszHostName = m_pszLocalName;
		pServer->StartOnThread(m_pszLocalName,pszPort);
	}
	else
	{
		pServer->StartOnThread(pszHost,pszPort);
	}
	m_ServerList.push_back(pServer);

	return pServer;
}


Client* 
NetworkManager::StartClient(const char* pszHost,const char* pszPort)
{
	m_pClient = new Client();
	m_pClient->StartOnThread(pszHost,pszPort);

	return m_pClient;
}

void 
NetworkManager::Update()
{
	m_bGameSideDone = true;

	for(uint32 i = 0; i < m_ServerList.size(); ++i)
	{
		if(m_ServerList[i] == nullptr)
				continue;

		if(m_ServerList[i]->GetStatus() == ServerStatus::RUNNING)
		{
			m_ServerList[i]->NetworkUpdate();
			m_ServerList[i]->GetScene()->NetworkUpdate();
		}
	}

	if(m_pClient != nullptr && m_pClient->GetScene() != nullptr)
	{
		if(m_pClient->GetStatus() == ClientStatus::CONNECTED)
		{
			m_pClient->NetworkUpdate();
			m_pClient->GetScene()->NetworkUpdate();
		}
	}
}


void NetworkManager::SetGameSideDone(bool bDone)
{
	m_bGameSideDone = bDone;
}
	
bool NetworkManager::IsGameSideDone()
{
	return m_bGameSideDone;
}

void NetworkManager::NetworkLock()
{
	++m_nNetworkLock;
}
	
void NetworkManager::NetworkUnlock()
{
	--m_nNetworkLock;
}