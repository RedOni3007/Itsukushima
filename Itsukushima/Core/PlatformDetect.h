/**
* Detect the platform, 86/64, debug/release/final
*
* @author: Kai Yang
**/

#ifdef _WIN32
	#define X_PC 1
	#define X_32 1
#elif _WIN64
	#define X_PC 1
	#define X_64 1
#endif

#ifdef _DEBUG
	#define X_DEBUG 1
#elif _RELEASE
	#define X_RELEASE 1
#elif _FINAL
	#define X_FINAL 1
#endif