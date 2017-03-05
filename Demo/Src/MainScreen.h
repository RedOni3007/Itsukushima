/*
* Enter point of all screens, like a placeholder
*
* @author: Kai Yang
*/
#ifndef MAINSCREEN_H
#define MAINSCREEN_H

class MainScreen : public Screen
{
public:
	MainScreen(void);
	virtual ~MainScreen(void);

	virtual void Init();
	virtual void Destroy();
	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual void Draw();
	virtual void ProcessInput();
};

#endif