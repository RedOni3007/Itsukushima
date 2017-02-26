/**
* To handle all the OS/platform stuff
* To handle the Top-level game loop
*
* @author: Kai Yang
**/

#ifndef GAMEAPP_H
#define	GAMEAPP_H

#include <Core/CoreHeaders.h>
#include <Core/GLEW_GLFW.h>
#include "ScreenManager.h"

#define GAMEAPP GameApp::Instance()

class GameApp
{
private:
	GameApp();
	~GameApp();

	float32		m_fLastLogicTime;
	float32		m_fLastGraphicTime;
	float32		m_fLogicTimePerFrame;
	float32		m_fGraphicTimePerFrame;
	int32		m_nLogicFPSLimit;
	int32		m_nGraphicFPSLimit;
	int32		m_nFrameCount;//graphic update count
	float32		m_fFPSTime;

	bool		m_bFrameByFrame;
	bool		m_bUpdateLogic;

	int32		m_nWindowWidth;	
	int32		m_nWindowHeight;	

public:
	static GameApp* Instance();
	static void Create();
	static void Destroy();

	static void Keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButton_callback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPos_callback(GLFWwindow* window, double x, double y);
	static void Scroll_callback(GLFWwindow* window, double x, double y);

	static void WindowSize_callback(GLFWwindow* window, int width, int height);

	bool Init(Screen* pStartScreen);
	void End();

	bool Update();

	//RunOneFrame ignores fps limit
	bool RunOneFrame();

	void SetFPSLimit(int32 fLogicLimit, int32 fGraphicLimit);

	int32 GetWindowWidth();
	int32 GetWindowHeight();

private:
	void LogicUpdate();
	void GraphicUpdate();
	void Draw();
};

#endif