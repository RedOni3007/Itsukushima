/*
* The Basement of each team
*
* @author: Kai Yang
*/

#ifndef BASEMENT_H
#define BASEMENT_H

class RemoteIO;
class GameServer;
struct NS_Base;

class Basement : public GameObjectComponent
{
public:
	Basement(void);
	virtual ~Basement(void);

	virtual void PreLogicUpdate();
	virtual void LogicUpdate();
	virtual void PostLogicUpdate();
	virtual void GraphicUpdate();

	virtual const char* GetName();
	static const char* ClassName();

	virtual void OnCollision(GameObject* pCollidedObj, CollisionInfo* pCollisionInfo);

public:
	void Init(Vector3 vPos, int32 nTeamNum, GameServer* pServer);

	int32 GetTeamID();

	void ProcessData(NS_Base* pData);

	void TakeDamage(int32 nDamage);

	void SetHealth(int nHealth);

private:
	void Respawn();

private:
	int32		m_nTeamNum;

	GameServer* m_pServer;

	GameObject* m_pPlaceholder;


	int32		m_nHealth;

	bool		m_bSpawning;
	float32		m_fSpawningCD;
};

#endif