#include "Itsukushima_include.h"

#include "MainScreen.h"


#if X_RELEASE
	//#pragma comment (linker, "/subsystem:windows /entry:mainCRTStartup") 
#endif

#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3
#define CORE_PROFILE 1

GLFWwindow* g_pMainWindow = nullptr;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main( int argc, char* argv[] )
{
	/*for(int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}*/

	GlobalSetting::Init();
	GlobalSetting::bOverrideHostName = true;
	GlobalSetting::bTrueGame = false;
	if(argc == 2)
	{
		GlobalSetting::bTrueGame = !StringHelper::Equals(argv[1],"0");
	}
	else if(argc == 3)
	{
		GlobalSetting::pszHostName = argv[1];
		GlobalSetting::pszPort = argv[2];
		GlobalSetting::bOverrideHostName = false;
	}
	else if(argc == 4)
	{
		GlobalSetting::pszHostName = argv[1];
		GlobalSetting::pszPort = argv[2];
		GlobalSetting::bOverrideHostName = false;
		GlobalSetting::bTrueGame = !StringHelper::Equals(argv[3],"0");
	}

	srand (time(NULL));
	glfwSetErrorCallback(error_callback);

	if(!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_RED_BITS, 8);
	//glfwWindowHint(GLFW_GREEN_BITS, 8);
	//glfwWindowHint(GLFW_BLUE_BITS, 8);
	//glfwWindowHint(GLFW_ALPHA_BITS, 8);
	//glfwWindowHint(GLFW_DEPTH_BITS, 32);
	//glfwWindowHint(GLFW_STENCIL_BITS, 32);

#ifdef X_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

#if CORE_PROFILE
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	g_pMainWindow = glfwCreateWindow(1280, 720, "Sumeragi", nullptr, nullptr);


	if (!g_pMainWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

	glfwMakeContextCurrent(g_pMainWindow);

	glfwSetKeyCallback(g_pMainWindow, GameApp::Keyboard_callback);
	glfwSetMouseButtonCallback(g_pMainWindow, GameApp::MouseButton_callback);
	glfwSetCursorPosCallback(g_pMainWindow, GameApp::CursorPos_callback);
	glfwSetScrollCallback(g_pMainWindow, GameApp::Scroll_callback);
	glfwSetWindowSizeCallback(g_pMainWindow, GameApp::WindowSize_callback);	

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

#if X_DEBUG
	Debug::Log("This error is from glew, ingore it.\n");
#endif
	printOpenGLError();//WILL receive a error from glew

	GameApp::Create();

	GAMEAPP->Init(new MainScreen());

	while (!glfwWindowShouldClose(g_pMainWindow))
    {
		glfwPollEvents();

		if(!GAMEAPP->Update())
			break;
    }

	GameApp::Destroy();

    glfwDestroyWindow(g_pMainWindow);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
