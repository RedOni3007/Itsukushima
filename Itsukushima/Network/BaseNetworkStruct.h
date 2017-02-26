/*
* BaseNetworkStruct
* Structs on engine side
* For game side to override
*
* @author: Kai Yang
*/

#ifndef BASE_NETWORK_STRUCT_H
#define BASE_NETWORK_STRUCT_H

#include <Core/CoreHeaders.h>


struct NS_Base
{
	int32	targetID;//who to process this data
	uint32	timeStamp;//decide on game side
	int32	nType;//should be the enum number

	NS_Base()
	{
		targetID = -1;
		timeStamp = 0;
		nType = -1;

	}

	virtual ~NS_Base()
	{

	}
};

struct NS_DataPacket
{
	int32 type;
	int32 size;//size of data
	uint8 data[NS_PACKET_DATA_MAX_SIZE];
};

struct NS_Data : RefCountBase
{
	int32 size;
	int8* pData;

	NS_Data(int32 _size, int8* _pData)
	{
		size = _size;
		pData = _pData;
	}

	virtual ~NS_Data()
	{
		if(pData != nullptr)
		{
			delete[] pData;
			pData = nullptr;
		}

		size = 0;
	}
};

#endif