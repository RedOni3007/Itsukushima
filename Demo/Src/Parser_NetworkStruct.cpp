#include "Parser_NetworkStruct.h"


void
NetworkStructParser::ToDataPacket(NS_Base* pOrigin, NS_DataPacket* pDataPack)
{
	if(pOrigin->nType == (int32)NetWorkStructEnum::ASSIGN_ID)
	{
		pDataPack->type = (int32)NetWorkStructEnum::ASSIGN_ID;
		pDataPack->size = sizeof(NS_AssignID);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::PLAYER_JOIN)
	{
		pDataPack->type = (int32)NetWorkStructEnum::PLAYER_JOIN;
		pDataPack->size = sizeof(NS_PlayerJoin);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::PLAYER_LEAVE)
	{
		pDataPack->type = (int32)NetWorkStructEnum::PLAYER_LEAVE;
		pDataPack->size = sizeof(NS_PlayerLeave);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::PLAYER_STATUS)
	{
		pDataPack->type = (int32)NetWorkStructEnum::PLAYER_STATUS;
		pDataPack->size = sizeof(NS_PlayerStatus);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::PLAYER_ATTACK)
	{
		pDataPack->type = (int32)NetWorkStructEnum::PLAYER_ATTACK;
		pDataPack->size = sizeof(NS_PlayerAttack);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::BASEMENTS_STATUS)
	{
		pDataPack->type = (int32)NetWorkStructEnum::BASEMENTS_STATUS;
		pDataPack->size = sizeof(NS_BasementStatus);
	}
	else if(pOrigin->nType == (int32)NetWorkStructEnum::OTHER_PlAYER_INIT)
	{
		pDataPack->type = (int32)NetWorkStructEnum::OTHER_PlAYER_INIT;
		pDataPack->size = sizeof(NS_OtherPlayerInit);
	}
	else
	{
		Debug::Assert(false, "undefined type");
	}

	Debug::Assert(pDataPack->size < NS_PACKET_DATA_MAX_SIZE);
	memcpy(&pDataPack->data[0], pOrigin, pDataPack->size);
}