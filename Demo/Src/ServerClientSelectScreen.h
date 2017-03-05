/*
* The screen for select to start a server or client
*
* @author: Kai Yang
*/
#ifndef SERVER_CLIENT_SELECT_SCREEN_H
#define SERVER_CLIENT_SELECT_SCREEN_H

class Server;
class Client;

class ServerClientSelectScreen : public Screen
{
public:
	ServerClientSelectScreen(void);
	virtual ~ServerClientSelectScreen(void);

	virtual void Init();
	virtual void Destroy();
	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual void Draw();
	virtual void ProcessInput();

	void EnterDemoScreen(Server* pServer, Client* pClient);
};

#endif