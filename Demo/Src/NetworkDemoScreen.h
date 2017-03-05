/*
* The screen for network demo
*
* @author: Kai Yang
*/
#ifndef NETWORKDEMOSCREEN_H
#define NETWORKDEMOSCREEN_H

class Scene;
class Camera;

class Server;
class Client;

class NetworkDemoScreen : public Screen
{
public:
	NetworkDemoScreen(Server* pServer, Client* pClient);
	virtual  ~NetworkDemoScreen();

	virtual void Init();
	virtual void Destroy();
	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual void Draw();
	virtual void ProcessInput();

private:
	Scene*		m_pScene;
	Camera*		m_pCamera;
	Server*		m_pServer;
	Client*		m_pClient;

	bool		m_bIsServer;
};

#endif