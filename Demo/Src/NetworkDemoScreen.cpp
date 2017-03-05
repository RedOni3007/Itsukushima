#include "Itsukushima_include.h"
#include "NetworkDemoScreen.h"
#include "GameServer.h"
#include "RemotePlayer.h"
#include "GameClient.h"
#include "Basement.h"

#define SINGLE_TEST 0

NetworkDemoScreen::NetworkDemoScreen(Server* pServer, Client* pClient)
{
	Debug::Assert((pServer!= nullptr && pClient != nullptr) == false, "Can't be server and client both, but might be ok in the future");

	m_pszName = "NetworkDemoScreen";
	m_pScene = nullptr;

	m_pServer = pServer;
	m_pClient = pClient;

	if(m_pServer != nullptr)
		m_bIsServer = true;
}


NetworkDemoScreen::~NetworkDemoScreen(void)
{
	delete m_pScene;
	m_pScene = nullptr;
}

void NetworkDemoScreen::Init()
{
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/floor_diffuse.png","floor_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/floor_specular.png","floor_specular");

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_diffuse.png","cross_diffuse");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_specular.png","cross_specular");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/cross_normal.png","cross_normal");

	Material *pMaterial = ResourceManager::Instance()->CreateMaterial("Floor");
	pMaterial->vSpecularColour = Vector3(0.5f,0.5f,0.5f);
	if(m_pServer != nullptr)
	{
		//a much dark floor, so I can find out which is server faster
		pMaterial->vDiffuseColour = Vector3(0.5f,0.5f,0.5f);
	}
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("floor_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_black");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Solid");
	pMaterial->bMetallic = 0;
	pMaterial->vSpecularColour = Vector3(0.5f,0.5f,0.5f);
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");

	pMaterial = ResourceManager::Instance()->CreateMaterial("Team1");
	pMaterial->bMetallic = 0;
	pMaterial->vSpecularColour = Vector3(0.5f,0.5f,0.5f);
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");
	pMaterial->vDiffuseColour = Vector3(1,0,0);

	pMaterial = ResourceManager::Instance()->CreateMaterial("Team2");
	pMaterial->bMetallic = 0;
	pMaterial->vSpecularColour = Vector3(0.5f,0.5f,0.5f);
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");
	pMaterial->vDiffuseColour = Vector3(0,0,1);

	pMaterial = ResourceManager::Instance()->CreateMaterial("Selected");
	pMaterial->bMetallic = 0;
	pMaterial->vSpecularColour = Vector3(0.5f,0.5f,0.5f);
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");
	pMaterial->vDiffuseColour = Vector3(0,1,0);

	pMaterial = ResourceManager::Instance()->CreateMaterial("Cross");
	pMaterial->bMetallic = 0;
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_diffuse");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_specular");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("cross_normal");

	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cube_low.obj","CUBE_LOW");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Sphere.obj","SPHERE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/D4.obj","D4");
	
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cube_low.obj","Box_Collision");
	
	
	m_pScene = new Scene();
	m_pScene->InitCollisionCollection(1024,128);

	m_pScene->SetGlobalLightDir(MFD_Normalize(Vector3(1.0f, 4.0f, 1.0f)));

	m_pCamera = new Camera();
	m_pCamera->SetFOV(60);

	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	RigidBody* pRigidBody = nullptr;

	pObject = m_pScene->CreateGameObject("floor");
	pObject->SetPos(Vector3(0.0,0.0f,0.0f));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(40,1,20));
	pModel = pObject->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	pObject->CreateColliderComponent("Box_Collision");
	pRigidBody = pObject->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);

	//create network part
	GameServer* pGameServer = nullptr;
	if(m_pServer != nullptr)
	{
		//free camera for server
		pObject = m_pScene->CreateGameObject("Camera");
		pObject->SetPos(0.0f,25.0f,10.0f);
		pObject->AddComponent(m_pCamera);
		DefaultCameraControl *pCameraControl = new DefaultCameraControl();
		pObject->AddComponent(pCameraControl);
		pCameraControl->SetCamera(m_pCamera);
		pCameraControl->SetMoveSpeed(0.2f);
		pCameraControl->SetTurnSpeed(0.2f);
		pCameraControl->SetTarget(0.0f,0.0f,0.0f);

		//create server object
		pObject = m_pScene->CreateGameObject("Server");//must call server until I find a solution inherit parentclass's name
		pGameServer = new GameServer();
		pObject->SetServerControlComponent(pGameServer);

		//give the scene to server
		m_pServer->SetScene(m_pScene);
	}

	if(m_pClient != nullptr)
	{
		pObject = m_pScene->CreateGameObject("camera");
		pObject->SetPos(0.0f,22.0f,5.0f);
		pObject->AddComponent(m_pCamera);
		DefaultCameraControl *pCameraControl = new DefaultCameraControl();
		pObject->AddComponent(pCameraControl);
		pCameraControl->SetCamera(m_pCamera);
		pCameraControl->SetMoveSpeed(0.0f);//no free move camera for clinet side
		pCameraControl->SetTurnSpeed(0.0f);//no free move camera for clinet side
		pCameraControl->SetTarget(0.0f,0.0f,0.0f);
		
		pObject = m_pScene->CreateGameObject("Client");//must call Client until I find a solution inherit parentclass's name
		GameClient* pGameClient = new GameClient();
		pObject->SetClientControlComponent(pGameClient);

		//give the scene to server
		m_pClient->SetScene(m_pScene);

#if SINGLE_TEST
		{
			Vector2 vMinBound(-19,-9);
			Vector2 vMaxBound(19,9);

			GameObject* pObject = m_pScene->CreateGameObject("test1");
			RemotePlayer* pRemotePlayer = new RemotePlayer();
			pObject->AddComponent(pRemotePlayer);
			pRemotePlayer->Init(1,Vector3(-2,1,0), 1, -1, true);
			pRemotePlayer->SetBound(vMinBound,vMaxBound);

			pObject = m_pScene->CreateGameObject("test2");
			pRemotePlayer = new RemotePlayer();
			pObject->AddComponent(pRemotePlayer);
			pRemotePlayer->Init(1,Vector3(2,1,0), 2, -1, false);
			pRemotePlayer->SetBound(vMinBound,vMaxBound);
		}
#endif
	}

	pObject = m_pScene->CreateGameObject("Base_Team1");
	Basement* pBasement = new Basement();
	pObject->AddComponent(pBasement);
	pBasement->Init(Vector3(-17,0.5f,0), 1, pGameServer);

	pObject = m_pScene->CreateGameObject("Base_Team2");
	pBasement = new Basement();
	pObject->AddComponent(pBasement);
	pBasement->Init(Vector3(17,0.5f,0), 2, pGameServer);

}

void NetworkDemoScreen::Destroy()
{
	//todo:unload resources or to unload package if I am going to pack resources into packages
}

void NetworkDemoScreen::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	float32 fTime = Timer::Instance()->GetTime();
	
	m_pScene->PreLogicUpdate();
	m_pScene->LogicUpdate();
	m_pScene->PostLogicUpdate();
}

void NetworkDemoScreen::GraphicUpdate()
{
	m_pScene->GraphicUpdate();
}

void NetworkDemoScreen::Draw()
{

	RenderManager::Instance()->RenderScene(m_pScene, m_pCamera);
}

void NetworkDemoScreen::ProcessInput()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	VPad pad = InputManager::Instance()->GetPad(0);
	

}