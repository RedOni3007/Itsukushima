/*
* RemotePlayer
* can be remote controlled, but not must
*
* @author: Kai Yang
*/

#ifndef REMOTE_PLAYER_H
#define REMOTE_PLAYER_H

class RemoteIO;
class GameServer;
class GameClient;
struct NS_Base;
struct NS_DataPacket;

class RemotePlayer : public GameObjectComponent
{
public:
	RemotePlayer(void);
	virtual ~RemotePlayer(void);

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();

	virtual const char* GetName();
	static const char* ClassName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

public:
	void Init(int32 nID,Vector3 vSpawnPos, int32 nTeamNum, GameServer* pGameServer, GameClient*  pGameclient,int32 nSocketFD, bool bIsLocalPlayer = false);

	int32 GetSocketFD();
	int32 GetID();
	int32 GetTeamID();

	RemoteIO* GetRemoteIO();

	void Disconnect();

	void ReceiveData(NS_DataPacket* pData);

	void TakeDamage(int32 nDamage);
	void SetHealth(int32 nHealth);

	void SetBound(Vector2& vMin, Vector2& vMax);

	void ForceRespawn();

private:
	void Respawn();

	void ProcessInput();

	void Attack();

	void ProcessNetwrokData();
	void ProcessData(NS_Base* pData);

private:
	GameServer*  m_pGameServer;
	GameClient*  m_pGameclient;

	int32		m_nID;
	int32		m_nSocketFD;

	Vector3		m_vSpawnPos;
	int32		m_nTeamNum;

	bool		m_bAlive;
	float32     m_fSpawnCD;

	GameObject* m_pAvatarBody;
	GameObject* m_pDirectionMark;
	GameObject* m_pAttackArm1;
	GameObject* m_pAttackArm2;
	GameObject* m_pPlayerMark;

	bool		m_bGoingToInit;
	bool		m_bInited;


	bool		m_bOnServer;
	bool		m_bIsLocalPlayer;
	RemoteIO*   m_pRemoteIO;

	float		m_fMoveSpeed;

	bool		m_bSpawning;
	float32		m_fSpawningCD;

	bool		m_bAttacking;
	float32		m_fAttackCD;

	int32		m_nHealth;

	Vector2		m_vMinBound;
	Vector2		m_vMaxBound;
};

#endif