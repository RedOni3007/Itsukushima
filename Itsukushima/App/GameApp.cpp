#include <App/GameApp.h>

#include <Render/RenderManager.h>
#include <App/ScreenManager.h>
#include <Input/InputManager.h>
#include <Resource/ResourceManager.h>
#include <Network/NetworkManager.h>

#include <Input/InputMapper_glfw.h>
#include <App/Timer.h>

#include <AntTweakBar.h>

#define RAW_INPUT_LOG 0

#define LOGIC_CATCHUP 0

#define FPS_TITLE 1
#define REALTIME_FPS 1

#define FRAMERATE 60

GameApp* g_pGameApp = nullptr;
extern GLFWwindow* g_pMainWindow;

GameApp::GameApp()
{
	assert(g_pGameApp == nullptr);

	m_fLastLogicTime = 0;
	m_fLastGraphicTime = 0;
	m_nFrameCount = 0;
	m_fFPSTime = 0;

	m_bUpdateLogic = true;
	m_bFrameByFrame = false;

	m_nWindowWidth = m_nWindowHeight = 0;

	SetFPSLimit(FRAMERATE,FRAMERATE);

	Timer::Instance()->SetMaxDeltaTime(0.1f);
}

GameApp::~GameApp()
{

}

GameApp* GameApp::Instance()
{
	assert(g_pGameApp != nullptr && "create first");

	return g_pGameApp;
}

void GameApp::Create()
{
	if(g_pGameApp == nullptr)
		g_pGameApp = new GameApp();

}

void GameApp::Destroy()
{
	if(g_pGameApp != nullptr)
	{
		g_pGameApp->End();
		delete g_pGameApp;
	}
}

int32 
GameApp::GetWindowWidth()
{
	return m_nWindowWidth;
}

int32 
GameApp::GetWindowHeight()
{
	return m_nWindowHeight;
}

bool GameApp::Init(Screen* pStartScreen)
{
	glfwGetWindowSize(g_pMainWindow,&m_nWindowWidth,&m_nWindowHeight);

	//init AntTweak
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(m_nWindowWidth, m_nWindowHeight);
	TwDefine("GLOBAL help='This is help text.' "); // Message added to the help bar.

	InputManager::Instance()->Init();
	ResourceManager::Instance()->Init();
	RenderManager::Instance()->Init();
	ScreenManager::Instance()->Init();
	NetworkManager::Instance()->Init();
	ScreenManager::Instance()->PushScreen(pStartScreen);
	return true;
}


void GameApp::End()
{
	//destroy networkmanager first,
	NetworkManager::Instance()->Destroy();

	//better in the reversed order of creation
	ScreenManager::Instance()->Destroy();

	TwTerminate();
}

bool GameApp::Update()
{
	static float32 fLastTime = (float32)glfwGetTime();
	float32 fCurrentTime = (float32)glfwGetTime();
	float32 fDelta = fCurrentTime - fLastTime;

	//calculate FPS
	m_fFPSTime += fDelta;
	if(m_fFPSTime >= 1.0f)
	{
		Timer::Instance()->SetFPS(m_nFrameCount);
			
#if FPS_TITLE && !REALTIME_FPS
		char pszTitle[NAME_CBUFFER_SIZE];
		sprintf_s(&pszTitle[0], NAME_CBUFFER_SIZE,"FPS = %d",m_nFrameCount);
		glfwSetWindowTitle(g_pMainWindow, pszTitle);
#endif
		m_nFrameCount = 0;
		m_fFPSTime = 0.0f;
	}

#if FPS_TITLE && REALTIME_FPS
		char pszTitle[NAME_CBUFFER_SIZE];
		sprintf_s(&pszTitle[0], NAME_CBUFFER_SIZE,"FPS = %f %d",fDelta, Timer::Instance()->GetFPS());
		glfwSetWindowTitle(g_pMainWindow, pszTitle);
#endif

	fLastTime = fCurrentTime;

	Timer::Instance()->SetTime(fCurrentTime);
	Timer::Instance()->SetDeltaTime(m_fLogicTimePerFrame);//fixed



	bool bLogicalUpdated = false;
	if(m_fLastLogicTime != 0)
	{
		fDelta = fCurrentTime - m_fLastLogicTime;

		if(fDelta > Timer::Instance()->GetMaxDeltaTime())
			fDelta = Timer::Instance()->GetMaxDeltaTime();

		if(fDelta>m_fLogicTimePerFrame)
		{
			bLogicalUpdated = true;

#if LOGIC_CATCHUP
			//this catch up could end slower and slower if the logic update causes the delay, not right
			do
			{
				LogicUpdate();
				fDelta -= m_fLogicTimePerFrame;
			}
			while(fDelta > m_fLogicTimePerFrame);
			m_fLastLogicTime = fCurrentTime - fDelta;
#else
			LogicUpdate();
			m_fLastLogicTime = fCurrentTime - (fDelta - m_fLogicTimePerFrame);
#endif
		}
		else
		{
			//just wait
			//DEBUG_PRINTF(("\n NO LOGIC UPDATE %f %f",fCurrentTime,fDelta ));
		}
	}
	else
	{
		//for first update
		bLogicalUpdated = true;
		LogicUpdate();
		m_fLastLogicTime = fCurrentTime;
	}

	if(m_fLastGraphicTime != 0)
	{
		fDelta = fCurrentTime - m_fLastGraphicTime;

		//graphic do not catch up
		if(fDelta > m_fGraphicTimePerFrame)
		{
			GraphicUpdate();
			m_fLastGraphicTime = fCurrentTime - (fDelta- m_fGraphicTimePerFrame);
		}
		else
		{
			//do nothing
			//DEBUG_PRINTF(("\n NO GRAPHIC UPDATE %f %f",fCurrentTime,fDelta ));
		}
	}
	else
	{
		//for first update
		GraphicUpdate();
		m_fLastGraphicTime = fCurrentTime;
	}

	return true;
}

bool GameApp::RunOneFrame()
{
	float32 fCurrentTime = (float32)glfwGetTime();

	Timer::Instance()->SetTime(fCurrentTime);
	Timer::Instance()->SetDeltaTime(m_fLogicTimePerFrame);//fixed

	LogicUpdate();

	GraphicUpdate();

	return true;
}

void GameApp::LogicUpdate()
{
	//input update go first, and alway update
	InputManager::Instance()->Update();

	VPad pad = InputManager::Instance()->GetPad(0);
	if(pad.IsButtonPressed(VPAD_BUTTON_12))
	{
		m_bFrameByFrame = !m_bFrameByFrame;
		if(m_bFrameByFrame == false)
			m_bUpdateLogic = true;
	}

	if(m_bFrameByFrame 
		&& (pad.IsButtonDown(VPAD_BUTTON_SELECT) || pad.IsButtonPressed(VPAD_BUTTON_13)))
	{
		m_bUpdateLogic = true;
	}

	if(m_bUpdateLogic)
	{
		NetworkManager::Instance()->SetGameSideDone(false);
		ScreenManager::Instance()->LogicUpdate();
		RenderManager::Instance()->LogicUpdate();
	}

	InputManager::Instance()->PostUpdate();//always as well

	//everything updated, now let's update network things
	NetworkManager::Instance()->Update();

	if(m_bFrameByFrame)
	{
		m_bUpdateLogic = false;
	}
}


void GameApp::GraphicUpdate()
{
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glClearColor(0.2f, 0.3f, 0.5f, 1.0f);

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//update
	ResourceManager::Instance()->GraphicUpdate();
	ScreenManager::Instance()->GraphicUpdate();
	RenderManager::Instance()->GraphicUpdate();

	//then dtaw
	ScreenManager::Instance()->Draw();

	TwDraw();

	glfwSwapBuffers(g_pMainWindow);

	++m_nFrameCount;
}

void GameApp::SetFPSLimit(int32 nLogicLimit, int32 nGraphicLimit)
{
	m_nLogicFPSLimit = nLogicLimit;
	m_nGraphicFPSLimit = nGraphicLimit;
	assert(m_nLogicFPSLimit <= m_nGraphicFPSLimit);

	m_fLogicTimePerFrame = 1.0f / m_nLogicFPSLimit;
	m_fGraphicTimePerFrame = 1.0f / m_nGraphicFPSLimit;
}

void GameApp::Keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
#if RAW_INPUT_LOG
	Debug::Log(("KEY: key=%d action=%d scancode=%d mods=%d\n", key,action,scancode,mods));
#endif

	if( !TwEventKeyGLFW(key, action) )  // Send event to AntTweakBar
    {

#if X_DEBUG
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(g_pMainWindow, GL_TRUE);
#endif

		if(action == GLFW_PRESS)
		{
			InputManager::Instance()->ButtonDown(0,InputMapper::MapKeyToButton(key));
		}
		else if (action == GLFW_RELEASE)
		{
			InputManager::Instance()->ButtonUp(0,InputMapper::MapKeyToButton(key));
		}
	}
}

void GameApp::MouseButton_callback(GLFWwindow* window, int button, int action, int mods)
{
#if RAW_INPUT_LOG
	Debug::Log(("MOUSE: button=%d action=%d mods=%d\n", button,action,mods));
#endif
	if( !TwEventMouseButtonGLFW(button, action) )   // Send event to AntTweakBar
    {
        // Event has not been handled by AntTweakBar    
		if(action == GLFW_PRESS)
		{
			InputManager::Instance()->ButtonDown(0,InputMapper::MapMouseButtonToButton(button));
		}
		else if (action == GLFW_RELEASE)
		{
			InputManager::Instance()->ButtonUp(0,InputMapper::MapMouseButtonToButton(button));
		}
	}
}

void GameApp::CursorPos_callback(GLFWwindow* window, double x, double y)
{
#if RAW_INPUT_LOG
	Debug::Log(("MOUSE: x=%f y=%f \n", x,y));
#endif

	if(!TwEventMousePosGLFW(x, y))
	{
		//not on any AntTweak things
	}

	InputManager::Instance()->NewMousePos((float32)x, (float32)y);
}

void GameApp::Scroll_callback(GLFWwindow* window, double x, double y)
{
#if RAW_INPUT_LOG
	Debug::Log("Scroll: x=%f y=%f \n", x,y);
#endif

	static double s_dWheelPos = 0;//just for AntTweak
	s_dWheelPos += y;
	if(!TwEventMouseWheelGLFW(s_dWheelPos))
	{

	}
}

void GameApp::WindowSize_callback(GLFWwindow* window, int width, int height)
{
	GameApp::Instance()->m_nWindowWidth = width;
	GameApp::Instance()->m_nWindowHeight = height;

	TwWindowSize(width, height);
	RenderManager::Instance()->WindowResize(width,height);
}