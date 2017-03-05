/*
* Game Server
* overrided ServerControl
*
* @author: Kai Yang
*/

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include <list>

class RemotePlayer;
struct NS_Base;

class GameServer : public ServerControl
{
public:
	GameServer();
	virtual ~GameServer();

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();
	virtual void NetworkUpdate();

	virtual const char* GetName();
	static const char* ClassName();


public:
	virtual void AddNewClient(int32 nSocketFD);
	virtual void RemoveClient(int32 nSocketFD);

	virtual void ReceiveData(NS_DataPacket* pDataPacket);

	std::list<RemotePlayer*> m_Team1Players;
	std::list<RemotePlayer*> m_Team2Players;

private:
	void ProcessData(NS_Base* pData);

	int32 m_nNextID;

};

#endif