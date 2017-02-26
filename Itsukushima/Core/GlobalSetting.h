/*
* static public global settings
*
* @author: Kai Yang
*/

#ifndef GLOBAL_SETTING_H
#define GLOBAL_SETTING_H

class GlobalSetting
{
public:
	static bool bTrueGame;

	static bool bOverrideHostName;

	static char* pszHostName;
	
	static char* pszPort;


public:
	static void Init();

};



#endif