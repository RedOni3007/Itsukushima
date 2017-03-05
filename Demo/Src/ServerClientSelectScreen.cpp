#include "Itsukushima_include.h"
#include "ServerClientSelectScreen.h"
#include "NetworkDemoScreen.h"

#include <AntTweakBar.h>

#define SERVERHOST "localhost"
#define SERVERPORT "8964"

TwBar *g_selectBar = nullptr;

void TW_CALL ServerButton_Callback(void *clientData)
{ 
    ServerClientSelectScreen* pScreen = (ServerClientSelectScreen*) clientData;
	Server* pServer = NetworkManager::Instance()->StartServer(GlobalSetting::pszHostName,GlobalSetting::pszPort);
	pScreen->EnterDemoScreen(pServer,nullptr);
}

void TW_CALL Client_Callback(void *clientData)
{ 
    ServerClientSelectScreen* pScreen = (ServerClientSelectScreen*) clientData;
	Client* pClient = NetworkManager::Instance()->StartClient(GlobalSetting::pszHostName,GlobalSetting::pszPort);
	pScreen->EnterDemoScreen(nullptr, pClient);
}

ServerClientSelectScreen::ServerClientSelectScreen(void)
{
	m_pszName = "ServerClientSelectScreen";

}


ServerClientSelectScreen::~ServerClientSelectScreen(void)
{
	
}

void 
ServerClientSelectScreen::EnterDemoScreen(Server* pServer, Client* pClient)
{
	m_bDead = true;

	ScreenManager::Instance()->PushScreen(new NetworkDemoScreen(pServer,pClient));
}

void ServerClientSelectScreen::Init()
{
	g_selectBar = TwNewBar("Main");
	TwDefine(" Main label='Server or a Client' ");
	TwDefine(" Main fontSize=3 ");
	TwDefine(" Main position='400 200' ");
	TwDefine(" Main size='400 200' ");
	TwDefine(" Main valueswidth=fit ");

	TwAddButton(g_selectBar, "ServerButton", ServerButton_Callback, this, " label='Start as Server' help='Start as Server.' ");
	TwAddButton(g_selectBar, "ClientButton", Client_Callback, this, " label='Start as Client' help='Start as Client.' ");
}

void ServerClientSelectScreen::Destroy()
{
	TwDeleteBar(g_selectBar);
}

void ServerClientSelectScreen::LogicUpdate()
{
	
}

void ServerClientSelectScreen::GraphicUpdate()
{

}

void ServerClientSelectScreen::Draw()
{


}

void ServerClientSelectScreen::ProcessInput()
{
	

}