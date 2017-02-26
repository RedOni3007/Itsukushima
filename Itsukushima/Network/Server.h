/*
* Server, more like a server for a single match
* Todo: c++ std::thread creation is expensive, need to find a way to pool it
*
* @author: Kai Yang
*/

#ifndef SERVER_H
#define SERVER_H

#include <Core/CoreHeaders.h>
#include <list>
#include <vector>

enum class ServerStatus : int32
{
	NOT_STARTED = 0,
	READY,
	RUNNING,
	STOPING,
	STOPPED,
};

class Scene;
class ServerControl;
class RemoteIO;

struct fd_set;
struct NS_DataPacket;

class Server
{
public:
	Server(void);
	~Server(void);

	void StartOnThread(const char* pszHost,const char* pszPort);//this is the one to start, not run

	void StartSend();

	void End();

	ServerStatus GetStatus();

	int32 GetListenSocketFD();

	void SetScene(Scene* pScene);
	Scene* GetScene();

	void AddNewClient(int32 nSocketFD);
	void RemoveClient(int32 nSocketFD);

	void ReceivedData(int32 nSocketFD, char* pData, int32 nByteLen);

	bool IsStopped();

	void NetworkUpdate();

private:
	bool Run(const char* pszHost,const char* pszPort);

	bool SendingManageThread();

	bool UDP_Iamhere_thread();

	bool SendData(RemoteIO* pRemoteIO);//tcp


private:
	ServerStatus m_eStatus;//overall status
	ServerStatus m_eSendThreadStatus;//status for the sending manage thread

	fd_set* m_pMasterFDSet;

	int32 m_nMaxSocketFD;
	int32 m_nListenSocketFD;

	Scene* m_pScene;//connected clients are handled here, the scene does not have to be visualized
	ServerControl* m_pServerControl;

	std::list<RemoteIO*> m_sendQueue;

	int32 m_nCurrentDataLen;
	int32 m_nCurrentDataReceived;
	int8* m_pCurrentDataBuffer;

	NS_DataPacket* m_pTempDataPacket;
};

#endif