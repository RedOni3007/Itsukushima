#include <Core/GlobalSetting.h>

bool GlobalSetting::bTrueGame = false;
bool GlobalSetting::bOverrideHostName = false;
char* GlobalSetting::pszHostName = "localhost";
char* GlobalSetting::pszPort = "8964";

void
GlobalSetting::Init()
{
	GlobalSetting::bTrueGame = false;
	GlobalSetting::bOverrideHostName = false;
	GlobalSetting::pszHostName = "localhost";
	GlobalSetting::pszPort = "8964";
}