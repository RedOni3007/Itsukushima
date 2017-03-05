#include "Itsukushima_include.h"
#include "DemoScreen.h"
#include "ObjectPool.h"
#include "Launcher.h"



DemoScreen::DemoScreen(void)
{
	m_pszName = "DemoScreen";

	m_pCamera = nullptr;
	m_pLaunch1 = nullptr;
	m_pLaunch2 = nullptr;
	m_pCross1 = nullptr;
	m_pCross2 = nullptr;
	m_pScene = nullptr;
	pObjectPool = nullptr;

	m_fWallFloorDegree = 15;
}


DemoScreen::~DemoScreen(void)
{
	m_pCamera = nullptr;
	m_pLaunch1 = nullptr;
	m_pLaunch2 = nullptr;
	m_pCross1 = nullptr;
	m_pCross2 = nullptr;

	delete pObjectPool;
	pObjectPool = nullptr;

	delete m_pScene;
	m_pScene = nullptr;
}

void DemoScreen::Init()
{
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/box_diffuse.png","box_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/box_specular.png","box_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/box_normal.png","box_normal");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/floor_diffuse.png","floor_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/floor_specular.png","floor_specular");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/sphere_diffuse.png","sphere_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/sphere_specular.png","sphere_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/sphere_normal.png","sphere_normal");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cylinder_diffuse.png","cylinder_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cylinder_diffuse.png","cylinder_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cylinder_normal.png","cylinder_normal");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_diffuse.png","cross_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_specular.png","cross_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_normal.png","cross_normal");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/strange_diffuse.png","strange_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/strange_diffuse.png","strange_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/strange_normal.png","strange_normal");

	Material *pMaterial = ResourceManager::Instance()->CreateMaterial("Floor");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("floor_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("floor_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Sphere");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("sphere_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("sphere_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("sphere_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Box");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("box_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("box_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("box_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Cylinder");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("cylinder_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("cylinder_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("cylinder_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Strange");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("strange_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("strange_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("strange_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Cross");
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Solid");
	pMaterial->bMetallic = 0;
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");

	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cube_high.obj","CUBE_HIGH");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cube_med.obj","CUBE_MED");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cube_low.obj","CUBE_LOW");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Sphere.obj","SPHERE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cylinder_high.obj","CYLINDER",true);
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Plane.obj","PLANE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/D4.obj","D4");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cross.obj","CROSS");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Strange.obj","STRANGE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Launch.obj","LAUNCH");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Umbrella.obj","UMBRELLA");

	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cube_low.obj","Box_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cylinder_high.obj","Cylinder_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Plane.obj","Plane_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/D4.obj","D4_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/strange.obj","Strange_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cross.obj","Cross_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Umbrella.obj","Umbrella_Collision");
	
	
	m_pScene = new Scene();
	m_pScene->InitCollisionCollection(1024,128);

	pObjectPool = new ObjectPool();
	pObjectPool->Init(m_pScene,200);

	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	RigidBody* pRigidBody = nullptr;

	pObject = m_pScene->CreateGameObject("camera");
	pObject->SetPos(0.0f,20.0f,50.0f);
	m_pCamera = new Camera();
	pObject->AddComponent(m_pCamera);
	m_pCamera->SetFOV(60);
	DefaultCameraControl *pCameraControl = new DefaultCameraControl();
	pObject->AddComponent(pCameraControl);
	pCameraControl->SetCamera(m_pCamera);
	pCameraControl->SetMoveSpeed(0.2f);
	pCameraControl->SetTurnSpeed(0.2f);
	pCameraControl->SetTarget(0.0f,0.0f,0.0f);
	m_pCameraObject = pObject;


	m_pCross1 = m_pScene->CreateGameObject("cross1");
	m_pCross1->SetPos(Vector3(10.0f,1.5f,10.0f));
	m_pCross1->SetRot(Vector3(0,45,0));
	m_pCross1->SetScale(Vector3(11,2,11));
	pModel = m_pCross1->CreateModelComponent("CROSS");
	pModel->SetMaterial("Cross");
	m_pCross1->CreateColliderComponent("Cross_Collision");
	pRigidBody = m_pCross1->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	pRigidBody->SetAngularVelocity(Vector3(0,1.0f,0));

	m_pCross2 = m_pScene->CreateGameObject("cross2");
	m_pCross2->SetPos(Vector3(-10.0f,1.5f,10.0f));
	m_pCross2->SetRot(Vector3(0,0,0));
	m_pCross2->SetScale(Vector3(11,2,11));
	pModel = m_pCross2->CreateModelComponent("CROSS");
	pModel->SetMaterial("Cross");
	m_pCross2->CreateColliderComponent("Cross_Collision");
	pRigidBody = m_pCross2->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	pRigidBody->SetAngularVelocity(Vector3(0,-1.0f,0));

	m_pCross3 = m_pScene->CreateGameObject("cross3");
	m_pCross3->SetPos(Vector3(10.0f,1.5f,-10.0f));
	m_pCross3->SetRot(Vector3(0,0,0));
	m_pCross3->SetScale(Vector3(11,2,11));
	pModel = m_pCross3->CreateModelComponent("CROSS");
	pModel->SetMaterial("Cross");
	m_pCross3->CreateColliderComponent("Cross_Collision");
	pRigidBody = m_pCross3->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	pRigidBody->SetAngularVelocity(Vector3(0,-1.0f,0));

	m_pCross4 = m_pScene->CreateGameObject("cross4");
	m_pCross4->SetPos(Vector3(-10.0f,1.5f,-10.0f));
	m_pCross4->SetRot(Vector3(0,45,0));
	m_pCross4->SetScale(Vector3(11,2,11));
	pModel = m_pCross4->CreateModelComponent("CROSS");
	pModel->SetMaterial("Cross");
	m_pCross4->CreateColliderComponent("Cross_Collision");
	pRigidBody = m_pCross4->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	pRigidBody->SetAngularVelocity(Vector3(0,1.0f,0));


	m_pLaunch1 = m_pScene->CreateGameObject("launch1");
	m_pLaunch1->SetPos(Vector3(-10,10,0));
	m_pLaunch1->SetRot(Vector3(0,0,-75));
	m_pLaunch1->SetScale(Vector3(0.75f));
	pModel = m_pLaunch1->CreateModelComponent("LAUNCH");
	pModel->SetMaterial("Cross");
	Launcher *pLauncher = new Launcher();
	pLauncher->SetObjectPool(pObjectPool);
	pLauncher->SetStartVelocity(Vector3(10,5,0));
	m_pLaunch1->AddComponent(pLauncher);

	m_pLaunch2 = m_pScene->CreateGameObject("launch2");
	m_pLaunch2->SetPos(Vector3(10,10,0));
	m_pLaunch2->SetRot(Vector3(0,0,75));
	m_pLaunch2->SetScale(Vector3(0.75f));
	pModel = m_pLaunch2->CreateModelComponent("LAUNCH");
	pModel->SetMaterial("Cross");
	pLauncher = new Launcher();
	pLauncher->SetObjectPool(pObjectPool);
	pLauncher->SetStartVelocity(Vector3(-10,5,0));
	m_pLaunch2->AddComponent(pLauncher);


	GameObject* m_pFloor = nullptr;
	m_pFloor = m_pScene->CreateGameObject("floor1");
	m_pFloor->SetPos(Vector3(0, 0,0));
	m_pFloor->SetRot(Vector3(0,0,0));
	m_pFloor->SetScale(Vector3(40,1,40));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);

	float32 fWallFloorWidth = 80;
	float32 fWallFloorHeight = 60;
	m_pFloor = m_pScene->CreateGameObject("WallFloor1");
	m_pFloor->SetPos(Vector3(-40, -10, 0));
	m_pFloor->SetRot(Vector3(0, 0, -m_fWallFloorDegree));
	m_pFloor->SetScale(Vector3(fWallFloorHeight,1,fWallFloorWidth));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	m_wallFloors.push_back(m_pFloor);

	m_pFloor = m_pScene->CreateGameObject("WallFloor2");
	m_pFloor->SetPos(Vector3(40, -10, 0));
	m_pFloor->SetRot(Vector3(0, 0, m_fWallFloorDegree));
	m_pFloor->SetScale(Vector3(fWallFloorHeight,1,fWallFloorWidth));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	m_wallFloors.push_back(m_pFloor);

	m_pFloor = m_pScene->CreateGameObject("WallFloor3");
	m_pFloor->SetPos(Vector3(0, -10, -40));
	m_pFloor->SetRot(Vector3(m_fWallFloorDegree, 0, 0));
	m_pFloor->SetScale(Vector3(fWallFloorWidth,1,fWallFloorHeight));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	m_wallFloors.push_back(m_pFloor);

	m_pFloor = m_pScene->CreateGameObject("WallFloor4");
	m_pFloor->SetPos(Vector3(0, -10, 40));
	m_pFloor->SetRot(Vector3(-m_fWallFloorDegree, 0, 0));
	m_pFloor->SetScale(Vector3(fWallFloorWidth,1,fWallFloorHeight));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);
	m_wallFloors.push_back(m_pFloor);

	m_pFloor = m_pScene->CreateGameObject("bottom_floor");
	m_pFloor->SetPos(Vector3(0, -35, 0));
	m_pFloor->SetRot(Vector3(0, 0, 0));
	m_pFloor->SetScale(Vector3(100,1,100));
	pModel = m_pFloor->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	m_pFloor->CreateColliderComponent("Box_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);

	m_pFloor = m_pScene->CreateGameObject("spin_Umbrella");
	m_pFloor->SetPos(Vector3(0, -30, 0));
	m_pFloor->SetRot(Vector3(0, 0, 0));
	m_pFloor->SetScale(Vector3(30.0f,20.0f,30.0f));
	pModel = m_pFloor->CreateModelComponent("UMBRELLA");
	pModel->SetMaterial("Cross");
	m_pFloor->CreateColliderComponent("Umbrella_Collision");
	pRigidBody = m_pFloor->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Cone);
	pRigidBody->SetAngularVelocity(Vector3(0.0f, 1.0f, 0.0f));

}

void DemoScreen::SetWallFloorAngle(float32 fDegree)
{
	GameObject* pObject = m_wallFloors[0];
	pObject->SetRot(Vector3(0, 0, -fDegree));

	pObject = m_wallFloors[1];
	pObject->SetRot(Vector3(0, 0, fDegree));

	pObject = m_wallFloors[2];
	pObject->SetRot(Vector3(fDegree, 0, 0));

	pObject = m_wallFloors[3];
	pObject->SetRot(Vector3(-fDegree, 0, 0));
}

void DemoScreen::Destroy()
{
	//todo:unload resources or to unload package if I am going to pack resources into packages
}

void DemoScreen::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	float32 fTime = Timer::Instance()->GetTime();
	
	m_pScene->PreLogicUpdate();
	m_pScene->LogicUpdate();
	m_pScene->PostLogicUpdate();
}

void DemoScreen::GraphicUpdate()
{
	m_pScene->GraphicUpdate();
}

void DemoScreen::Draw()
{
	//glEnable(GL_CULL_FACE);

	RenderManager::Instance()->RenderScene(m_pScene, m_pCamera);
}

void DemoScreen::ProcessInput()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	VPad pad = InputManager::Instance()->GetPad(0);
	
	if(pad.IsButtonDown(VPAD_BUTTON_10))
	{
		m_fWallFloorDegree += 0.1f;
		if(m_fWallFloorDegree > 90)
			m_fWallFloorDegree = 90;

		SetWallFloorAngle(m_fWallFloorDegree);
	}
	else if(pad.IsButtonDown(VPAD_BUTTON_11))
	{
		m_fWallFloorDegree -= 0.1f;
		if(m_fWallFloorDegree < 0)
			m_fWallFloorDegree = 0;

		SetWallFloorAngle(m_fWallFloorDegree);
	}

	if(pad.IsButtonPressed(VPAD_BUTTON_MOUSE1))
	{
		Vector2 vMousePos = InputManager::Instance()->GetMousePos();
		Vector3 vRayPos;
		Vector3 vRayDir;
		Vector3 vHitPos;

		m_pCamera->CalculateScreenToWorldRay(vMousePos.x,vMousePos.y,GAMEAPP->GetWindowWidth(), GAMEAPP->GetWindowHeight(), vRayPos, vRayDir);

		GameObject* pObject = m_pScene->RayHit(vRayPos,vRayDir,m_pCamera->GetFarClip(),vHitPos);

		if(pObject != nullptr)
		{
			if(pObject->GetRigidBodyComponent() != nullptr)
			{
				pObject->GetRigidBodyComponent()->LeaveResting();
				pObject->GetRigidBodyComponent()->AddForce(vHitPos,vRayDir * 100.0f);
			}
		}
	}

}