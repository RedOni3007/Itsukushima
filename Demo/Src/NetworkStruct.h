/*
* NetworkDataStruct
* Structs for game side behaves
*
* @author: Kai Yang
*/

#ifndef NETWORK_STRUCT_H
#define NETWORK_STRUCT_H

#include "Itsukushima_include.h"

enum class NetWorkStructEnum : int32
{
	ASSIGN_ID = 0,
	PLAYER_JOIN,
	PLAYER_LEAVE,
	PLAYER_STATUS,
	PLAYER_ATTACK,
	OTHER_PlAYER_INIT,
	BASEMENTS_STATUS,
	NetWorkStructEnum_COUNT,
};

struct NS_AssignID : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::ASSIGN_ID;
	int32	id;

	NS_AssignID()
	{
		nType = (int32) eType;
	}
};

struct NS_PlayerJoin : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::PLAYER_JOIN;

	int32		id;
	int32		team;
	Vector3		spawnPos;

	NS_PlayerJoin()
	{
		nType = (int32) eType;
	}
};

struct NS_PlayerLeave : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::PLAYER_LEAVE;

	int32		id;

	NS_PlayerLeave()
	{
		nType = (int32) eType;
	}
};

struct NS_PlayerStatus : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::PLAYER_STATUS;

	Vector3		vPos;
	Quaternion	qRot;
	int32		nHealth;

	NS_PlayerStatus()
	{
		nType = (int32) eType;
	}
};

struct NS_PlayerAttack : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::PLAYER_ATTACK;

	int32		id;

	NS_PlayerAttack()
	{
		nType = (int32) eType;
	}
};

struct NS_BasementStatus : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::BASEMENTS_STATUS;

	int32		nTeamID;
	int32		nHealth;

	NS_BasementStatus()
	{
		nType = (int32) eType;
	}
};


//NS_PlayerJoin + NS_PlayerStatus
struct NS_OtherPlayerInit : NS_Base
{
	const static NetWorkStructEnum eType = NetWorkStructEnum::OTHER_PlAYER_INIT;

	int32		id;
	int32		team;
	Vector3		spawnPos;

	Vector3		vPos;
	Quaternion	qRot;
	int32		nHealth;

	NS_OtherPlayerInit()
	{
		nType = (int32) eType;
	}
};


#endif