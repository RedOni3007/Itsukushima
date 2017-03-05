/*
* NetworkStructPaser
*
* @author: Kai Yang
*/

#ifndef NETWORK_STRUCT_PASER_H
#define NETWORK_STRUCT_PASER_H

#include "NetworkStruct.h"

class NetworkStructParser
{
public:
	static void ToDataPacket(NS_Base* pOrigin, NS_DataPacket* pDataPack);
};


#endif