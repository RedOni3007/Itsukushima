#include "DebugOutput.h"

#define IGNORE_DEBUG 1


void 
Debug::Log(const char *pszFormat, ...)
{
#if X_DEBUG || IGNORE_DEBUG
	static char buffer[0xffff];//opengl error are huge sometime
    va_list args;
    va_start (args, pszFormat);
    vsprintf_s (buffer, pszFormat, args);
    printf("%s", buffer);
    va_end (args);
#endif
}

void 
Debug::LogValue(const char* valueName, int32 value)
{
	Debug::Log("%s = %d\n", valueName, value);
}

void 
Debug::LogValue(const char* valueName, uint32 value)
{
	Debug::Log("%s = %d\n", valueName, value);
}

void 
Debug::LogValue(const char* valueName, float32 value)
{
	Debug::Log("%s = %f\n", valueName, value);
}

void 
Debug::LogValue(const char* valueName, Vector2 value)
{
	Debug::Log("%s = %f, %f\n", valueName, value.x, value.y);
}

void 
Debug::LogValue(const char* valueName, Vector3 value)
{
	Debug::Log("%s = %f, %f, %f\n", valueName, value.x, value.y,value.z);
}

void 
Debug::LogValue(const char* valueName, Vector4 value)
{
	Debug::Log("%s = %f, %f, %f, %f\n", valueName, value.x, value.y,value.z, value.w);
}

void 
Debug::Assert(bool bTrue, const char* pszStr)
{
	if(bTrue)
		return;

	//should always set a breakpoint here, for myself
	if(pszStr != nullptr)
		Debug::Log(pszStr);
}