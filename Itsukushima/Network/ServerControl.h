/*
* ServerControlComponent
* Server behave on game side
* This is default behave(empty), should be overrided to create game side behave
*
* @author: Kai Yang
*/

#ifndef SERVER_CONTROL_COMPONENT_H
#define SERVER_CONTROL_COMPONENT_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>

class Server;
struct NS_DataPacket;

class ServerControl : public GameObjectComponent
{
public:
	ServerControl();
	virtual ~ServerControl();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual void NetworkUpdate();

	virtual const char* GetName();
	static const char* ClassName();

	virtual void ReceiveData(NS_DataPacket* pDataPacket);

	virtual void AddNewClient(int32 nSocketFD);
	virtual void RemoveClient(int32 nSocketFD);

	void SetServer(Server* pServer);
	
	void AddGlobalData(NS_DataPacket* pDataPacket);

protected:
	virtual void StartSend();
	virtual void PackData();

protected:
	Server* m_pServer;

	std::list<NS_DataPacket*> m_globalSendQueue;
};

#endif