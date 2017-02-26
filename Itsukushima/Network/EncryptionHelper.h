/*
* Encryption helper
*
* @author: Kai Yang
*/

#ifndef ENCRYPTION_HELPER_H
#define ENCRYPTION_HELPER_H

#include <Core/CoreHeaders.h>

class EncryptionHelper
{
public:
	static void EncryptXOR(int8* pData, int32 nDataLen);
	static void DecryptXOR(int8* pData, int32 nDataLen);
};


#endif