#include <Core/StringHelper.h>
#include <Core/CoreHeaders.h>
#include <string>

StringHelper::StringHelper(void)
{
}


StringHelper::~StringHelper(void)
{
}

bool 
StringHelper::EndWith(const char* pszStr, const char* pszToken)
{
	int32 nStrlength = strlen(pszStr);
	int32 nTokenLength = strlen(pszToken);

	if(nTokenLength > nStrlength)
		return false;

	int32 nOffset = nStrlength - nTokenLength;

	for(int32 i = 0; i < nTokenLength; ++i)
	{
		if(pszToken[i] != pszStr[i + nOffset])
			return false;
	}

	return true;
}

bool 
StringHelper::StartWith(const char* pszStr, const char* pszToken)
{
	int32 nStrlength = strlen(pszStr);
	int32 nTokenLength = strlen(pszToken);

	if(nTokenLength > nStrlength)
		return false;

	for(int32 i = 0; i < nTokenLength; ++i)
	{
		if(pszToken[i] != pszStr[i])
			return false;
	}

	return true;
}

bool 
StringHelper::Equals(const char* pszStr1, const char* pszStr2)
{
	return strcmp(pszStr1,pszStr2) == 0;
}

int32 
StringHelper::Split(const char* pszStr, const char cToken, std::vector<char*> results, int32 nLen)
{
	int32 nStrlength = strlen(pszStr);
	int32 nStart = 0;
	uint32 nTokenCount = 0;
	int32 nCurrentTokenLen = 0;
	char* pCurrentStr = nullptr;
	for(int32 i = 0; i < nStrlength; ++i)
	{
		if(nTokenCount >= results.size())
			break;

		if(pszStr[i] == cToken || pszStr[i] == '\0' || i == nStrlength-1)
		{
			nCurrentTokenLen = i - nStart;
			if(i == nStrlength-1)
				nCurrentTokenLen += 1;

			if(nCurrentTokenLen <= 0)
				continue;

			pCurrentStr = results[nTokenCount];
			memcpy(&pCurrentStr[0], &pszStr[nStart], nCurrentTokenLen);
			pCurrentStr[nCurrentTokenLen] = '\0';

			nStart = i + 1;
			++nTokenCount;
		}
	}

	return nTokenCount;
}