#include <App/ScreenManager.h>


ScreenManager::ScreenManager(void)
{
}


ScreenManager::~ScreenManager(void)
{
}

ScreenManager* ScreenManager::Instance()
{
	static ScreenManager me;
	return &me;
}

void ScreenManager::Init()
{

}

void ScreenManager::Destroy()
{
	std::vector<Screen*>::iterator it = m_ScreenList.begin();
	while(it != m_ScreenList.end())
	{		
		if(!(*it)->isStatic())
		{
			(*it)->Destroy();
			delete (*it);
		}
		++it;	
	}

	m_ScreenList.clear();
}

void ScreenManager::PushScreen(Screen* pScreen)
{
	assert(pScreen != nullptr);
	pScreen->Init();
	m_ScreenList.push_back(pScreen);

	Debug::Log("ScreenManager: %s IN.\n",pScreen->GetName());
}

void ScreenManager::LogicUpdate()
{
	if(m_ScreenList.size() == 0)
		return;

	//only top screen can receive input
	(m_ScreenList.back())->ProcessInput();

	//remove any dead screen
	std::vector<Screen*>::iterator it = m_ScreenList.begin();
	while(it != m_ScreenList.end())
	{
		if((*it)->isDead())
		{
			if(!(*it)->isStatic())
			{
				Debug::Log("ScreenManager: %s OUT.\n",(*it)->GetName());
				(*it)->Destroy();
				delete (*it);
			}
			it = m_ScreenList.erase(it);
		}
		else
		{
			++it;
		}
	}

	//any new screen added in current frame will not update in current frame
	uint32 uCurrentScreenCount = m_ScreenList.size();
	for(uint32 i = 0; i < uCurrentScreenCount; ++i)
	{
		m_ScreenList[i]->LogicUpdate();
	}
}

void ScreenManager::GraphicUpdate()
{
	std::vector<Screen*>::iterator it = m_ScreenList.begin();

	while(it != m_ScreenList.end())
	{
		if(!(*it)->isDead())
			(*it)->GraphicUpdate();

		++it;
	}
}

void ScreenManager::Draw()
{
	std::vector<Screen*>::iterator it = m_ScreenList.begin();

	while(it != m_ScreenList.end())
	{
		if(!(*it)->isDead())
			(*it)->Draw();

		++it;
	}
}