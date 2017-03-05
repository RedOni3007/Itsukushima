#include "Itsukushima_include.h"
#include "DamageDemoScreen.h"
#include "ObjectGenerator.h"

#include "CutBladeObjControl.h"
#include "CutBlade.h"

DamageDemoScreen::DamageDemoScreen(void)
{
	m_pszName = "DamageDemoScreenScreen";

	m_pCamera = nullptr;
	m_pScene = nullptr;

	m_pBladeObject = nullptr;
	m_pBlade = nullptr;
}


DamageDemoScreen::~DamageDemoScreen(void)
{
	m_pCamera = nullptr;

	m_pBladeObject = nullptr;
	m_pBlade = nullptr;

	delete m_pScene;
	m_pScene = nullptr;	
}

void DamageDemoScreen::Init()
{
	glEnable(GL_CULL_FACE);

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
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");

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

	pMaterial = ResourceManager::Instance()->CreateMaterial("Solid_blue");
	pMaterial->bMetallic = 0;
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");
	pMaterial->vDiffuseColour = Vector3(0.0f,0.0f,1.0f);

	pMaterial = ResourceManager::Instance()->CreateMaterial("Solid_red");
	pMaterial->bMetallic = 0;
	pMaterial->nDiffuseTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nSpecularTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_white");
	pMaterial->nNormalTextureIndex = ResourceManager::Instance()->GetTextureIndex("solid_normal");
	pMaterial->vDiffuseColour = Vector3(1.0f,0.0f,0.0f);

	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cube_low.obj","CUBE_LOW");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Sphere.obj","SPHERE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cylinder_high.obj","CYLINDER",true);
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/CutPlane.obj","PLANE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/D4.obj","D4");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Cross.obj","CROSS");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Strange.obj","STRANGE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Launch.obj","LAUNCH");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Umbrella.obj","UMBRELLA");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Blade.obj","BLADE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Concave.obj","CONCAVE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Star2.obj","STAR");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Tree.obj","TREE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Table.obj","TABLE");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/Chair.obj","CHAIR");
	ResourceManager::Instance()->LoadMesh("../Assets/Mesh/S.obj","S");

	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cube_low.obj","Box_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Sphere_collider.obj","Sphere_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cylinder_high.obj","Cylinder_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/CutPlane.obj","Plane_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/D4.obj","D4_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/strange.obj","Strange_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Cross.obj","Cross_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Umbrella.obj","Umbrella_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Blade_Collider.obj","Blade_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Concave.obj","Concave_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Star2.obj","Star_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Tree_collider.obj","Tree_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Table.obj","Table_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/Chair.obj","Chair_Collision");
	ResourceManager::Instance()->LoadCollisionMesh("../Assets/Mesh/S.obj","S_Collision");

	m_pScene = new Scene();
	m_pScene->InitCollisionCollection(1024,128);

	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	RigidBody* pRigidBody = nullptr;

	pObject = m_pScene->CreateGameObject("camera");
	pObject->SetPos(-1.0f, 3.0f, -3.0f);
	m_pCamera = new Camera();
	pObject->AddComponent(m_pCamera);
	m_pCamera->SetFOV(60);
	DefaultCameraControl *pCameraControl = new DefaultCameraControl();
	pCameraControl->Enable(false);
	pObject->AddComponent(pCameraControl);
	pCameraControl->SetCamera(m_pCamera);
	pCameraControl->SetMoveSpeed(0.2f);
	pCameraControl->SetTurnSpeed(0.2f);
	pCameraControl->SetDirection(0.0f,-0.5f,1.0f);//make camera face the same direction of gameobject
	//pCameraControl->SetTarget(0.0f,0.0f, 0.0f);
	m_pCameraObject = pObject;

	pObject = m_pScene->CreateGameObject("floor1");
	pObject->SetPos(Vector3(0, 0,0));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(40,1,40));
	pModel = pObject->CreateModelComponent("CUBE_LOW");
	pModel->SetMaterial("Floor");
	pObject->CreateColliderComponent("Box_Collision");
	pRigidBody = pObject->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	pRigidBody->SetMomentInertiaType(InertiaType::Box);

	ObjectGenerator* pGenerator = nullptr;
	pObject = m_pScene->CreateGameObject("Generator1");
	pObject->SetPos(Vector3(-6,5,0));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object1");
	pGenerator->SetModelName("CONCAVE");
	pGenerator->SetMaterialName("Box");
	pGenerator->SetColliderName("Concave_Collision");
	pGenerator->SetInertiaType(InertiaType::Box);
	pGenerator->SetRotation(Vector3(0,90,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator2");
	pObject->SetPos(Vector3(6,5,0));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object2");
	pGenerator->SetModelName("TABLE");
	pGenerator->SetMaterialName("Strange");
	pGenerator->SetColliderName("Table_Collision");
	pGenerator->SetInertiaType(InertiaType::Box);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator3");
	pObject->SetPos(Vector3(0,5,0));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object3");
	pGenerator->SetModelName("CUBE_LOW");
	pGenerator->SetMaterialName("Box");
	pGenerator->SetColliderName("Box_Collision");
	pGenerator->SetInertiaType(InertiaType::Box);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator4");
	pObject->SetPos(Vector3(3,5,0));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object4");
	pGenerator->SetModelName("CYLINDER");
	pGenerator->SetMaterialName("Cylinder");
	pGenerator->SetColliderName("Cylinder_Collision");
	pGenerator->SetInertiaType(InertiaType::Cylinder);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator5");
	pObject->SetPos(Vector3(-3,5,0));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object5");
	pGenerator->SetModelName("STAR");
	pGenerator->SetMaterialName("Strange");
	pGenerator->SetColliderName("Star_Collision");
	pGenerator->SetInertiaType(InertiaType::Cylinder);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator6");
	pObject->SetPos(Vector3(6,5,7));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object6");
	pGenerator->SetModelName("TREE");
	pGenerator->SetMaterialName("Cross");
	pGenerator->SetColliderName("Tree_Collision");
	pGenerator->SetInertiaType(InertiaType::Cylinder);
	pGenerator->SetRotation(Vector3(0,30,0));
	pGenerator->SeScale(Vector3(3,1,2));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator7");
	pObject->SetPos(Vector3(3,5,7));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object7");
	pGenerator->SetModelName("SPHERE");
	pGenerator->SetMaterialName("Cross");
	pGenerator->SetColliderName("Sphere_Collision");
	pGenerator->SetInertiaType(InertiaType::Sphere);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator8");
	pObject->SetPos(Vector3(0,5,7));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object8");
	pGenerator->SetModelName("CHAIR");
	pGenerator->SetMaterialName("Box");
	pGenerator->SetColliderName("Chair_Collision");
	pGenerator->SetInertiaType(InertiaType::Box);
	pGenerator->SetRotation(Vector3(0,90,0));
	pGenerator->SeScale(Vector3(1,1,1));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator9");
	pObject->SetPos(Vector3(-3,5,7));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object9");
	pGenerator->SetModelName("SPHERE");
	pGenerator->SetMaterialName("Cross");
	pGenerator->SetColliderName("Sphere_Collision");
	pGenerator->SetInertiaType(InertiaType::Ellipsoid);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1.5f, 1.0f, 0.5f));
	pObject->AddComponent(pGenerator);

	pObject = m_pScene->CreateGameObject("Generator10");
	pObject->SetPos(Vector3(-6,5,7));
	pGenerator = new ObjectGenerator();
	pGenerator->SetObjectName("object10");
	pGenerator->SetModelName("S");
	pGenerator->SetMaterialName("Strange");
	pGenerator->SetColliderName("S_Collision");
	pGenerator->SetInertiaType(InertiaType::Ellipsoid);
	pGenerator->SetRotation(Vector3(0,0,0));
	pGenerator->SeScale(Vector3(1.0f, 1.0f, 1.0f));
	pObject->AddComponent(pGenerator);


	pObject = m_pScene->CreateGameObject("bladeObject");
	pObject->SetPos(Vector3(0,1,-2.5f));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(1,1,1));
	pModel = pObject->CreateModelComponent("SPHERE");
	pModel->SetMaterial("Solid");
	pObject->CreateColliderComponent(0.5f);
	pRigidBody = pObject->CreateRigidBodyComponent();
	pRigidBody->SetStatic(true);
	CutBladeObjControl *pBladeObjControl = new CutBladeObjControl();
	pBladeObjControl->SetCamera(m_pCamera);
	pObject->AddComponent(pBladeObjControl);
	m_pBladeObject = pObject;
	m_pCameraObject->SetParent(m_pBladeObject);

	GameObject* pRealBladeHolderRoll = nullptr;
	pObject = m_pScene->CreateGameObject("realBladeHolder");
	pObject->SetPos(Vector3(0,0,0));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(1.0f,1.0f,1.0f));
	pObject->SetParent(m_pBladeObject);
	pRealBladeHolderRoll = pObject;

	GameObject* pRealBladeHolderYaw = nullptr;
	pObject = m_pScene->CreateGameObject("realBladeHolder");
	pObject->SetPos(Vector3(0,0,0));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(1.0f,1.0f,1.0f));
	pObject->SetParent(pRealBladeHolderRoll);
	pRealBladeHolderYaw = pObject;

	GameObject* pRealBladeHolder = nullptr;
	pObject = m_pScene->CreateGameObject("realBladeHolder");
	pObject->SetPos(Vector3(0,0,0));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(1.0f,1.0f,1.0f));
	pObject->SetParent(pRealBladeHolderYaw);
	pRealBladeHolder = pObject;

	GameObject* pRealBlade = nullptr;
	pObject = m_pScene->CreateGameObject("realBlade");
	pObject->SetPos(Vector3(0,1.2f,0));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(1.0f,1.5f,1.0f));
	pModel = pObject->CreateModelComponent("BLADE");
	pModel->SetMaterial("Solid_red");
	pObject->CreateColliderComponent("Blade_Collision");
	m_pBlade = new CutBlade();
	m_pBlade->Init();
	pObject->AddComponent(m_pBlade);
	m_pBlade->SetHolderObecjt(pRealBladeHolder);
	m_pBlade->SetHolderObecjt_Yaw(pRealBladeHolderYaw);
	m_pBlade->SetHolderObecjt_Roll(pRealBladeHolderRoll);
	pObject->SetParent(pRealBladeHolder);
	pRealBlade = pObject;

	pObject = m_pScene->CreateGameObject("aimPlane");
	pObject->SetPos(Vector3(0, 0.0f, 1.1f));
	pObject->SetRot(Vector3(0,0,0));
	pObject->SetScale(Vector3(0.001f,4.2f, 1.8f));
	pModel = pObject->CreateModelComponent("CUBE_LOW");
	pModel->SetRenderPass(RenderPass::GBUFFER,false);
	pModel->SetRenderPass(RenderPass::TOUCH_HIGHLIGHT,true);
	//pModel->SetRenderPass(RenderPass::TRANSPARENT,true);
	pModel->SetMaterial("Solid");
	pModel->SetTransparency(0.1f);
	pObject->SetParent(pRealBladeHolderYaw);

}

void DamageDemoScreen::Destroy()
{
	//todo:unload resources or to unload package if I am going to pack resources into packages
}

void DamageDemoScreen::LogicUpdate()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	float32 fTime = Timer::Instance()->GetTime();
	
	m_pScene->PreLogicUpdate();
	m_pScene->LogicUpdate();
	m_pScene->PostLogicUpdate();
}

void DamageDemoScreen::GraphicUpdate()
{
	m_pScene->GraphicUpdate();
}

void DamageDemoScreen::Draw()
{
	RenderManager::Instance()->RenderScene(m_pScene, m_pCamera);
}

void DamageDemoScreen::ProcessInput()
{
	float32 fDeltaTime = Timer::Instance()->GetDeltaTime();
	VPad pad = InputManager::Instance()->GetPad(0);

	if(pad.IsButtonPressed(VPAD_BUTTON_SELECT))
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

	if(pad.IsButtonPressed(VPAD_BUTTON_14))
	{
		static bool bCullBack = true;

		bCullBack = !bCullBack;
		if(bCullBack)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if(pad.IsButtonPressed(VPAD_BUTTON_15))
	{
		static bool bFramewire = false;

		bFramewire = !bFramewire;
		RenderManager::Instance()->EnableFramewire(bFramewire);
	}

	if(pad.IsButtonPressed(VPAD_BUTTON_16))
	{
		static bool bBackFramewire = false;

		bBackFramewire = !bBackFramewire;
		RenderManager::Instance()->EnableBackFramewire(bBackFramewire);
	}

	if(pad.IsButtonPressed(VPAD_BUTTON_13))
	{
		m_pBlade->SwitchCutMethod();
	}
}