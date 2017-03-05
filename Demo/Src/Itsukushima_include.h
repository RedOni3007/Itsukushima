/**
* Includes for Itsukushima Engine
*
* @author: Kai Yang
**/

#include <Core/CoreHeaders.h>
#include <Core/GlobalSetting.h>
#include <Core/StringHelper.h>
#include <Game/GameObjectComponent.h>
#include <Game/Scene.h>
#include <Game/Camera.h>
#include <Game/DefaultCameraControl.h>
#include <App/Timer.h>
#include <App/ScreenManager.h>
#include <App/GameApp.h>
#include <Resource/ResourceManager.h>
#include <Resource/Model.h>
#include <Resource/Mesh.h>
#include <Physics/PhysicsStructs.h>
#include <Physics/RigidBody.h>
#include <Physics/Collider.h>
#include <Physics/CollisionManager.h>
#include <Physics/CutHelper.h>
#include <Network/NetworkManager.h>
#include <Network/RemoteIO.h>
#include <Network/BaseNetworkStruct.h>
#include <Network/ServerControl.h>
#include <Network/Client.h>
#include <Network/Server.h>
#include <Network/ClientControl.h>
#include <Render/RenderManager_Defines.h>
#include <Render/RenderManager.h>
#include <Render/OpenGL_ErrorCheck.h>
#include <Input/InputManager.h>
#include <Math/MathHelper.h>
#include <Math/CollisionHelper.h>

#include <glm/ext.hpp>
