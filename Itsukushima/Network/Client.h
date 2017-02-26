/*
* Client
* 
*
* @author: Kai Yang
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <Core/CoreHeaders.h>
#include <list>
#include <vector>

enum class ClientStatus : int32
{
	NOT_CONNECTED = 0,
	READY,
	RUNNING,
	CONNECTING,
	CONNECTED,
	STOPPING,
	STOPPED,
};

struct fd_set;
struct NS_DataPacket;
struct NS_Data;

class ClientControl;
class Scene;
class RemoteIO;


class Client
{
public:
	Client(void);
	~Client(void);

	bool Run(const char* pszHost,const char* pszPort);

	void StartOnThread(const char* pszHost,const char* pszPort);//this is the one to start, not run

	void StartSend();

	void End();

	ClientStatus GetStatus();

	void ReceivedData(char* pData, int32 nByteLen);

	void SetScene(Scene* pScene);
	Scene* GetScene();

	bool IsStopped();

	void NetworkUpdate();

private:
	bool SendingManageThread();

	bool SendData(RemoteIO* pRemoteIO);//tcp

	bool UDP_Listen_thread();

private:
	ClientStatus m_eConnectStatus;
	ClientStatus m_eSendThreadStatus;

	fd_set* m_pMasterFDSet;
	int32 m_nConnectedSocketfd;

	Scene* m_pScene;//connected clients are handled here, the scene does not have to be visualized
	ClientControl* m_pClientControl;

	int32 m_nCurrentDataLen;
	int32 m_nCurrentDataReceived;
	int8* m_pCurrentDataBuffer;

	NS_DataPacket* m_pTempDataPacket;

	std::list<RemoteIO*> m_sendQueue;

	bool m_bConnectionInfoFromUDP;
	bool m_bKillUDP;
	char m_pszHostFromUDP[NAME_CBUFFER_SIZE];
	char m_pszPortFromUDP[NAME_CBUFFER_SIZE];
};

#endif