/*
* Screen and Screen manager 
* todo:put screen into another file?
*
* @author: Kai Yang
*/
#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <Core/CoreHeaders.h>
#include <vector>

class Screen
{
public:
	Screen()
	{
		m_bActive = true;
		m_bVisible = true;
		m_bDead = false;
		m_bStatic = false;
		m_bInited = false;
	};

	virtual ~Screen(){};

	virtual void Init() = 0;
	virtual void Destroy() = 0;
	virtual void LogicUpdate() = 0;
	virtual void GraphicUpdate() = 0;
	virtual void Draw() = 0;

	virtual void ProcessInput() = 0;

	void setStatic(bool bStatic) { m_bStatic = bStatic; }
	bool isStatic() const { return m_bStatic; }

	bool isActive() const { return m_bActive; }
	bool isVisible() const { return m_bVisible; }
	bool isDead() const { return m_bDead; }

	const char* GetName(){return m_pszName;}

protected:
	const char* m_pszName;
	bool m_bActive;
	bool m_bVisible;
	bool m_bDead;
	bool m_bStatic;//if true, the screen will not be destroyed when remove from manager
	bool m_bInited;
};

class ScreenManager
{
private:
	ScreenManager(void);
	~ScreenManager(void);

public:
	static ScreenManager* Instance();

	void Init();
	void Destroy();
	void PushScreen(Screen* pScreen);

	void LogicUpdate();
	void GraphicUpdate();
	void Draw();

private:
	std::vector<Screen*>	m_ScreenList;
};

#endif