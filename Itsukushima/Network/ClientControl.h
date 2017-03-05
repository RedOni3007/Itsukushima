/*
* ClientControlComponent
* Client behave on game side
* This is default behave(empty) , should be overrided to create game side behave
*
* @author: Kai Yang
*/

#ifndef CLIENT_CONTROL_COMPONENT_H
#define CLIENT_CONTROL_COMPONENT_H

#include <Core/CoreHeaders.h>
#include <Game/GameObjectComponent.h>

class Client;
class RemoteIO;
struct NS_DataPacket;

class ClientControl : public GameObjectComponent
{
public:
	ClientControl();
	virtual ~ClientControl();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual void NetworkUpdate();

	virtual const char* GetName();
	static const char* ClassName();

	virtual void ReceiveData(NS_DataPacket* pDataPacket);

	void SetClient(Client* pClient);

	virtual void AddNewClient(int32 nSocketFD);
	virtual void RemoveClient(int32 nSocketFD);

	void AddGlobalData(NS_DataPacket* pDataPacket);

	RemoteIO* GetRemoteIO();

protected:
	virtual void StartSend();
	virtual void PackData();

protected:
	Client* m_pClient;

	std::list<NS_DataPacket*> m_globalSendQueue;

	RemoteIO* m_pRemoteIO;
};

#endif