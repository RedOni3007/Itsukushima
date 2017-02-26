#include "EncryptionHelper.h"
#include <string>

const char* pszKey = "Arsenal";
int32 nKeyLen = strlen(pszKey);

void 
EncryptionHelper::EncryptXOR(int8* pData, int32 nDataLen)
{
	for(int32 i = 0; i < nDataLen; ++i)
	{
		if(pData[i] != 0)
			pData[i] = pData[i] ^ pszKey[i%nKeyLen];
	}
}

void 
EncryptionHelper::DecryptXOR(int8* pData, int32 nDataLen)
{
	for(int32 i = 0; i < nDataLen; ++i)
	{
		if(pData[i] != 0)
			pData[i] = pData[i] ^ pszKey[i%nKeyLen];
	}
}