/*
* Network Manager
* Handle multiple servers for multiple games on "SERVER" side, 
* or a single client on "CLIENT" side 
*
* @author: Kai Yang
*/

#ifndef NETWORKMANAGER_H
#define NETWOEKMANAGER_H

#include <Core/CoreHeaders.h>
#include <vector>

#include "BaseNetworkStruct.h"

class Server;
class Client;

class NetworkManager
{
private:
	NetworkManager(void);
	~NetworkManager(void);


public:
	static NetworkManager* Instance();

	void Init();
	void Destroy();

	Server* StartServer(const char* pszHost,const char* pszPort);
	Client* StartClient(const char* pszHost,const char* pszPort);

	void Update();

	void SetGameSideDone(bool bDone);
	bool IsGameSideDone();

	void NetworkLock();
	void NetworkUnlock();

private:
	bool m_bInited;

	std::vector<Server*> m_ServerList;

	Client* m_pClient;//this is for client side, so it's only one

	char* m_pszLocalName;

	bool m_bGameSideDone;

	int32 m_nNetworkLock;
};

#endif