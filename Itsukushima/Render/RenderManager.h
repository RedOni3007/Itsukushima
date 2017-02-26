/*
* Rendering!!!
* todo: create just two vao for static and dynamic objects
* todo: to convert all those program, texture location and uniform things to class, 
*
* @author: Kai Yang
*/

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <Core/CoreHeaders.h>

#include <Core/GLEW_GLFW.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Render/RenderManager_Defines.h>

class Scene;
class Camera;
class FBOManager;
struct Texture;

class RenderManager
{

private:
	RenderManager();
	~RenderManager();

public:
	static RenderManager* Instance();

	void Init();

	void LogicUpdate();
	void GraphicUpdate();
	void RenderScene(Scene* pScene, Camera* pCamera);

	void WindowResize(int32 nWidth, int32 nHeight);

	void EnableFramewire(bool bEnable);
	void EnableBackFramewire(bool bEnable);

private:
	void DrawGBuffer(Scene* pScene, Camera* pCamera);
	void DrawLights(Scene* pScene, Camera* pCamera);
	void DrawTransparent(Scene* pScene, Camera* pCamera);
	void DrawTouchHighlight(Scene* pScene, Camera* pCamera);

private:
	GLuint m_uDiffuseTextureLoc;
	GLuint m_uSpecularTextureLoc;
	GLuint m_uNormalTextureLoc;

	GLuint m_uGBufferPosTexLoc_GlobalLight;
	GLuint m_uGBufferNormalTexLoc_GlobalLight;
	GLuint m_uGBufferColorTexLoc_GlobalLight; 
	GLuint m_uGBufferTexCoordTexLoc_GlobalLight;

	GLuint m_uGBufferPosTexLoc_SpotLight;
	GLuint m_uGBufferNormalTexLoc_SpotLight;
	GLuint m_uGBufferColorTexLoc_SpotLight; 
	GLuint m_uGBufferTexCoordTexLoc_SpotLight;

	GLuint m_uDiffuseTextureLoc_Transparent;
	GLuint m_uSpecularTextureLoc_Transparent; 
	GLuint m_uNormalTextureLoc_Transparent;

	GLuint m_uGBufferPosTexLoc_TouchHighlight;

	GLuint m_uPerModelUniformBufferID;
	GLuint m_uPerModelUniformBufferID_Transparent;
	GLuint m_uPerModelUniformBufferID_TouchHighlight;

	GLuint m_uPerFrameUniformBufferID_global;
	GLuint m_uPerFrameUniformBufferID_spot;

	GLuint m_uSpotLightUniformBufferID;

	GLuint m_uTouchHighlightUniformBufferID;

	GLuint m_uMaterialUniformBufferID;
	GLuint m_uMaterialUniformBufferID_Transparent;

	GLuint m_uGBufferProgramID;
	GLuint m_uGlobalLightProgramID;
	GLuint m_uSpotLightProgramID;
	GLuint m_uTransparentProgramID;
	GLuint m_uTouchHighlightProgramID;

	FBOManager* m_pFBOManager;

	GL_Mesh m_sFullscreenQuad;

	Texture* m_pSolidWhiteTex;
	Texture* m_pSolidBlackTex;
	Texture* m_pSolidNormalTex;

	bool m_bFramewire;
	bool m_bBackFramewire;
};

#endif