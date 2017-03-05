/*
* The screen for cutting demo
*
* @author: Kai Yang
*/

#ifndef DAMAGE_DEMO_SCREEN_H
#define DAMAGE_DEMO_SCREEN_H

class Scene;
class CollisionManager;
class ObjectPool;
class Camera;
class CutBlade;

class DamageDemoScreen  :	public Screen
{
public:
	DamageDemoScreen(void);
	virtual ~DamageDemoScreen(void);

	virtual void Init();
	virtual void Destroy();
	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual void Draw();
	virtual void ProcessInput();

private:
	Scene*  m_pScene;

	GameObject* m_pCameraObject;
	Camera* m_pCamera;

	GameObject* m_pBladeObject;

	CutBlade* m_pBlade;
};

#endif