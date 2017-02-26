/**
* Debug class, currently only the print functions
* todo: record all the output to file
* todo: make the output into where I can copy/paste
*
* @author: Kai Yang
**/

#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include <Core/CoreHeaders.h>
#include "PlatformDetect.h"
#include <stdarg.h>
#include <stdio.h>


class Debug
{
public:
	static void Log(const char *pszFormat, ...);
	static void LogValue(const char* valueName, int32 value);
	static void LogValue(const char* valueName, uint32 value);
	static void LogValue(const char* valueName, float32 value);
	static void LogValue(const char* valueName, Vector2 value);
	static void LogValue(const char* valueName, Vector3 value);
	static void LogValue(const char* valueName, Vector4 value);

	static void Assert(bool bTrue, const char* pszStr = "");
};

#endif