/*
* The screen for rigid body demo
*
* @author: Kai Yang
*/
#ifndef DEMOSCREEN_H
#define DEMOSCREEN_H

class Scene;
class CollisionManager;
class ObjectPool;
class Camera;

class DemoScreen :	public Screen
{
public:
	DemoScreen(void);
	virtual ~DemoScreen(void);

	virtual void Init();
	virtual void Destroy();
	virtual void LogicUpdate();
	virtual void GraphicUpdate();
	virtual void Draw();
	virtual void ProcessInput();

	void SetWallFloorAngle(float32 fDegree);

private:
	Scene*  m_pScene;

	GameObject* m_pCameraObject;
	Camera* m_pCamera;

	GameObject* m_pCross1;
	GameObject* m_pCross2;
	GameObject* m_pCross3;
	GameObject* m_pCross4;

	GameObject* m_pLaunch1;
	GameObject* m_pLaunch2;

	float32 m_fWallFloorDegree;
	std::vector<GameObject*> m_wallFloors;

	ObjectPool* pObjectPool;
};

#endif