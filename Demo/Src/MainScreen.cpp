#include "Itsukushima_include.h"
#include "MainScreen.h"
#include "DemoScreen.h"
#include "DamageDemoScreen.h"
#include "ServerClientSelectScreen.h"

MainScreen::MainScreen(void)
{
	m_pszName = "MainScreen";
}


MainScreen::~MainScreen(void)
{
}

void MainScreen::Init()
{

}

void MainScreen::Destroy()
{

}

void MainScreen::LogicUpdate()
{
	static int count = 0;

	if(--count > 0)
		return;

	if(m_bDead == false)
	{
		m_bDead = true;

		//rigid body
		ScreenManager::Instance()->PushScreen(new DemoScreen());

		//cut
		//ScreenManager::Instance()->PushScreen(new DamageDemoScreen());

		//network
		//ScreenManager::Instance()->PushScreen(new ServerClientSelectScreen());
	}
}

void MainScreen::GraphicUpdate()
{

}

void MainScreen::Draw()
{

}

void MainScreen::ProcessInput()
{

}