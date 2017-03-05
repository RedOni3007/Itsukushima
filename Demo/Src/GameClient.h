/*
* Game Client, game side client
* overrided ClientControl
*
* @author: Kai Yang
*/

#ifndef GAME_CLIENT_H
#define GAME_CLIENT_H


#include <list>

class RemotePlayer;
class RemoteIO;
class Basement;
struct NS_Base;

class GameClient : public ClientControl
{
public:
	GameClient(void);
	virtual ~GameClient(void);

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


private:
	void ProcessNetwrokData();
	void ProcessData(NS_Base* pData);

	std::list<RemotePlayer*> m_Team1Players;
	std::list<RemotePlayer*> m_Team2Players;

	Basement* m_Basement1;
	Basement* m_Basement2;

	int32 m_nServerSideID;
};

#endif