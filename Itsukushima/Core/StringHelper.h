/**
* Helper functions for char string
*
* @author: Kai Yang
**/

#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <Core/CoreHeaders.h>

#include <list>
#include <vector>

class StringHelper
{
private:
	StringHelper(void);
	~StringHelper(void);

public:
	static bool StartWith(const char* pszStr, const char* pszToken);
	static bool EndWith(const char* pszStr, const char* pszToken);

	static int32 Split(const char* pszStr, const char cToken, std::vector<char*> results, int32 nLen);

	static bool Equals(const char* pszStr1, const char* pszStr2);
};

#endif