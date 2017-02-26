#include <Render/RenderManager.h>

#include "OpenGL_ErrorCheck.h"

#include <Resource/ResourceManager.h>
#include <Game/Scene.h>
#include <Resource/Model.h>
#include <Resource/Mesh.h>
#include <Game/Camera.h>
#include <App/Timer.h>
#include <Game/GameObject.h>
#include "FBOManager.h"
#include <App/GameApp.h>

#define FRAMEWIRE 0
#define BACK_FRAMEWIRE 0

RenderManager::RenderManager()
{
	m_pFBOManager = nullptr;
	m_pSolidWhiteTex = nullptr;
	m_pSolidBlackTex = nullptr;
	m_pSolidNormalTex = nullptr;

	m_bFramewire = false;
	m_bBackFramewire = false;
}


RenderManager::~RenderManager()
{
	glDeleteBuffers(1, &m_sFullscreenQuad.vertex_vbo);
    glDeleteBuffers(1, &m_sFullscreenQuad.element_vbo);
    glDeleteVertexArrays(1, &m_sFullscreenQuad.vao);

	glDeleteProgram(m_uGBufferProgramID);
	glDeleteProgram(m_uGlobalLightProgramID);
	glDeleteProgram(m_uSpotLightProgramID);
}

RenderManager* 
RenderManager::Instance()
{
	static RenderManager me;
	return &me;
}

void 
RenderManager::EnableFramewire(bool bEnable)
{
	m_bFramewire = bEnable;
}
	
void 
RenderManager::EnableBackFramewire(bool bEnable)
{
	m_bBackFramewire = bEnable;
}

void
RenderManager::LogicUpdate()
{

}

void 
RenderManager::GraphicUpdate()
{

}

void 
RenderManager::Init()
{
	/*
     * Tutorial: this section of code creates a fullscreen quad to be used
     *           when computing global illumination effects (e.g. ambient)
     */
    {
        std::vector<glm::vec2> vertices(4);
        vertices[0] = glm::vec2(-1, -1);
        vertices[1] = glm::vec2(1, -1);
        vertices[2] = glm::vec2(1, 1);
        vertices[3] = glm::vec2(-1, 1);

        glGenBuffers(1, &m_sFullscreenQuad.vertex_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_sFullscreenQuad.vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(glm::vec2),
                     vertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &m_sFullscreenQuad.vao);
        glBindVertexArray(m_sFullscreenQuad.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_sFullscreenQuad.vertex_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              sizeof(glm::vec2), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

	m_pFBOManager = new FBOManager();
	
	//todo:create render tech class

	//gbuffer
	m_uGBufferProgramID = ResourceManager::Instance()->LoadShader("../Assets/Shader/gbuffer_vs.glsl","../Assets/Shader/gbuffer_fs.glsl",nullptr);
	m_uDiffuseTextureLoc  = glGetUniformLocation(m_uGBufferProgramID,"gSamplerDiffuse");
	m_uSpecularTextureLoc  = glGetUniformLocation(m_uGBufferProgramID,"gSamplerSpecular");
	m_uNormalTextureLoc  = glGetUniformLocation(m_uGBufferProgramID,"gSamplerNormal");
	printOpenGLError();	

	glGenBuffers(1,&m_uMaterialUniformBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uMaterialUniformBufferID);
	glUniformBlockBinding(m_uGBufferProgramID, glGetUniformBlockIndex(m_uGBufferProgramID,"MaterialUniforms"), 0);
	printOpenGLError();	

	glGenBuffers(1,&m_uPerModelUniformBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uPerModelUniformBufferID);
	glUniformBlockBinding(m_uGBufferProgramID, glGetUniformBlockIndex(m_uGBufferProgramID,"PerModelUniforms"), 1);
	printOpenGLError();

	//global light, not very useful for indoor scene, should be replaced by other lights later
	m_uGlobalLightProgramID = ResourceManager::Instance()->LoadShader("../Assets/Shader/light_global_vs.glsl","../Assets/Shader/light_global_fs.glsl",nullptr);
	m_uGBufferPosTexLoc_GlobalLight			= glGetUniformLocation(m_uGlobalLightProgramID,"sampler_world_position");
	m_uGBufferNormalTexLoc_GlobalLight		= glGetUniformLocation(m_uGlobalLightProgramID,"sampler_world_normal");
	m_uGBufferColorTexLoc_GlobalLight		= glGetUniformLocation(m_uGlobalLightProgramID,"sampler_diffuse");
	m_uGBufferTexCoordTexLoc_GlobalLight	= glGetUniformLocation(m_uGlobalLightProgramID,"sampler_specular");
	printOpenGLError();	

	glGenBuffers(1,&m_uPerFrameUniformBufferID_global);
	glBindBuffer(GL_UNIFORM_BUFFER,m_uPerFrameUniformBufferID_global);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameUniforms),nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_uPerFrameUniformBufferID_global);
	glUniformBlockBinding(m_uGlobalLightProgramID,glGetUniformBlockIndex(m_uGlobalLightProgramID,"PerFrameUniforms"),2);
	printOpenGLError();

	//spot light
	/*
	m_uSpotLightProgramID = ResourceManager::Instance()->LoadShader("../Assets/Shader/light_spot_vs.glsl","../Assets/Shader/light_spot_fs.glsl",nullptr);
	m_uGBufferPosTexLoc_SpotLight			= glGetUniformLocation(m_uSpotLightProgramID,"sampler_world_position");
	m_uGBufferNormalTexLoc_SpotLight		= glGetUniformLocation(m_uSpotLightProgramID,"sampler_world_normal");
	m_uGBufferColorTexLoc_SpotLight			= glGetUniformLocation(m_uSpotLightProgramID,"sampler_diffuse");
	m_uGBufferTexCoordTexLoc_SpotLight		= glGetUniformLocation(m_uSpotLightProgramID,"sampler_specular");
	printOpenGLError();	

	glGenBuffers(1,&m_uPerFrameUniformBufferID_spot);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uPerFrameUniformBufferID_spot);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameUniforms),nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_uPerFrameUniformBufferID_spot);
	glUniformBlockBinding(m_uSpotLightProgramID, glGetUniformBlockIndex(m_uSpotLightProgramID,"PerFrameUniforms"),3);
	printOpenGLError();

	glGenBuffers(1,&m_uSpotLightUniformBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER,m_uSpotLightUniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightUniforms),nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_uSpotLightUniformBufferID);
	glUniformBlockBinding(m_uSpotLightProgramID, glGetUniformBlockIndex(m_uSpotLightProgramID,"SpotLightUniforms"),4);
	printOpenGLError();
	*/

	//transpart
	m_uTransparentProgramID = ResourceManager::Instance()->LoadShader("../Assets/Shader/transparent_vs.glsl","../Assets/Shader/transparent_fs.glsl",nullptr);
	m_uDiffuseTextureLoc_Transparent	= glGetUniformLocation(m_uTransparentProgramID,"gSamplerDiffuse");
	m_uSpecularTextureLoc_Transparent	= glGetUniformLocation(m_uTransparentProgramID,"gSamplerSpecular");
	m_uNormalTextureLoc_Transparent		= glGetUniformLocation(m_uTransparentProgramID,"gSamplerNormal");
	printOpenGLError();	

	glGenBuffers(1,&m_uMaterialUniformBufferID_Transparent);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID_Transparent);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, m_uMaterialUniformBufferID_Transparent);
	glUniformBlockBinding(m_uTransparentProgramID, glGetUniformBlockIndex(m_uTransparentProgramID,"MaterialUniforms"), 5);
	printOpenGLError();	

	glGenBuffers(1,&m_uPerModelUniformBufferID_Transparent);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_Transparent);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, m_uPerModelUniformBufferID_Transparent);
	glUniformBlockBinding(m_uTransparentProgramID, glGetUniformBlockIndex(m_uTransparentProgramID,"PerModelUniforms"), 6);
	printOpenGLError();

	//touch highlight
	m_uTouchHighlightProgramID = ResourceManager::Instance()->LoadShader("../Assets/Shader/touch_highlight_vs.glsl","../Assets/Shader/touch_highlight_fs.glsl",nullptr);
	m_uGBufferPosTexLoc_TouchHighlight	= glGetUniformLocation(m_uTouchHighlightProgramID,"sampler_world_position");
	printOpenGLError();	

	glGenBuffers(1,&m_uTouchHighlightUniformBufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uTouchHighlightUniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(TouchHighlightUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, m_uTouchHighlightUniformBufferID);
	glUniformBlockBinding(m_uTouchHighlightProgramID, glGetUniformBlockIndex(m_uTouchHighlightProgramID,"TouchHighlightUniforms"), 7);
	printOpenGLError();	

	glGenBuffers(1,&m_uPerModelUniformBufferID_TouchHighlight);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_TouchHighlight);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 8, m_uPerModelUniformBufferID_TouchHighlight);
	glUniformBlockBinding(m_uTouchHighlightProgramID, glGetUniformBlockIndex(m_uTouchHighlightProgramID,"PerModelUniforms"), 8);
	printOpenGLError();

	glBindBuffer(GL_UNIFORM_BUFFER,0);
	printOpenGLError();

	m_pFBOManager->SetGBufferProgramID(m_uGBufferProgramID);
	m_pFBOManager->Init(GameApp::Instance()->GetWindowWidth(),GameApp::Instance()->GetWindowHeight());

	ResourceManager::Instance()->LoadTexture("../Assets/Texture/solid_white.png","solid_white");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/solid_black.png","solid_black");
	ResourceManager::Instance()->LoadTexture("../Assets/Texture/solid_normal.png","solid_normal");

	m_pSolidWhiteTex = ResourceManager::Instance()->GetTexture("solid_white");
	m_pSolidBlackTex = ResourceManager::Instance()->GetTexture("solid_black");
	m_pSolidNormalTex = ResourceManager::Instance()->GetTexture("solid_normal");
}

void 
RenderManager::WindowResize(int32 nWidth, int32 nHeight)
{
	m_pFBOManager->Resize(nWidth,nHeight);
}

void 
RenderManager::RenderScene(Scene* pScene, Camera* pCamera)
{
	DrawGBuffer(pScene,pCamera);
	DrawLights(pScene,pCamera);
	DrawTransparent(pScene,pCamera);
	DrawTouchHighlight(pScene,pCamera);

	//copy screen
	GLint viewport_size[4];
    glGetIntegerv(GL_VIEWPORT, viewport_size);
	m_pFBOManager->StartScreenBufferRead();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(viewport_size[0],viewport_size[1],viewport_size[2],viewport_size[3],
					  viewport_size[0],viewport_size[1],viewport_size[2],viewport_size[3],
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBindVertexArray(0);
	printOpenGLError();
}

void 
RenderManager::DrawGBuffer(Scene* pScene, Camera* pCamera)
{
	m_pFBOManager->StartGBufferWrite();
	glUniform1i(m_uDiffuseTextureLoc,0);
	glUniform1i(m_uSpecularTextureLoc,1);
	glUniform1i(m_uNormalTextureLoc,2);

	Vector3 vCameraPos = pCamera->GetPos();
	glUniform3f(glGetUniformLocation(m_uGBufferProgramID,"vCameraPos"),vCameraPos.x, vCameraPos.y, vCameraPos.z);

	Matrix44 mProjectMat = *pCamera->GetProjectMat();
	Matrix44 mViewMat = *pCamera->GetViewMat();

	PerModelUniforms sPerModelUniform;
	MaterialUniforms sMaterialUniform;
	Material *pMaterial;
	uint32 modelCount = pScene->GetObjectCount();
	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	Mesh* pMesh = nullptr;
	for(uint32 i = 0; i < modelCount; ++i)
	{
		pObject = pScene->GetGameObject(i);
		pModel = pObject->GetModelComponent();
		if(pModel == nullptr || pModel->IsEnabled() == false)
			continue;

		if(pModel->IsRenderPassEnabled(RenderPass::GBUFFER) == false)
			continue;

		pMesh = pModel->GetMesh();
		if(pMesh == nullptr)
			continue;

		pMaterial = ResourceManager::Instance()->GetMaterial(pModel->GetMaterialIndex());
			//setup textures
		if(pMaterial->nDiffuseTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nDiffuseTextureIndex);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}
		if(pMaterial->nSpecularTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nSpecularTextureIndex);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}
		if(pMaterial->nNormalTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nNormalTextureIndex);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}

		sMaterialUniform.vDiffuseColour = pMaterial->vDiffuseColour;
		sMaterialUniform.fShininess = pMaterial->fShininess;
		sMaterialUniform.vSpecularColour = pMaterial->vSpecularColour;
		sMaterialUniform.bMetallic = pMaterial->bMetallic;
		glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), &sMaterialUniform, GL_STREAM_DRAW);
		printOpenGLError();

		sPerModelUniform.WorldMatIn = *pObject->GetTranformMat();
		sPerModelUniform.mvpIn = mProjectMat * mViewMat * sPerModelUniform.WorldMatIn;
		sPerModelUniform.TintColor = pModel->GetTintColor();
		glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), &sPerModelUniform, GL_STREAM_DRAW);
		printOpenGLError();

		glBindVertexArray(pMesh->GetVAO());
		glDrawElements(GL_TRIANGLES, pMesh->GetIndexCount() ,GL_UNSIGNED_SHORT,(void*)0);

		printOpenGLError();
	}

	glBindVertexArray(0);
	printOpenGLError();
}

void 
RenderManager::DrawLights(Scene* pScene, Camera* pCamera)
{
	m_pFBOManager->StartScreenBufferWrite();

	glUseProgram(m_uGlobalLightProgramID);
	glUniform1i(m_uGBufferPosTexLoc_GlobalLight, m_pFBOManager->GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE_POSITION));
	glUniform1i(m_uGBufferNormalTexLoc_GlobalLight,m_pFBOManager->GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE_NORMAL));
	glUniform1i(m_uGBufferColorTexLoc_GlobalLight,m_pFBOManager->GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE_DIFFUSE));
	glUniform1i(m_uGBufferTexCoordTexLoc_GlobalLight,m_pFBOManager->GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE_SPECULAR));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE,m_pFBOManager->GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE_POSITION));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE,m_pFBOManager->GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE_NORMAL));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE,m_pFBOManager->GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE_DIFFUSE));

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_RECTANGLE,m_pFBOManager->GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE_SPECULAR));
	printOpenGLError();

	Matrix44 mProjectMat = *pCamera->GetProjectMat();
	Matrix44 mViewMat = *pCamera->GetViewMat();
	PerFrameUniforms sPerFrameUniforms;
	sPerFrameUniforms.vCameraPos = pCamera->GetPos();
	sPerFrameUniforms.vCameraDir = pCamera->GetDir();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uPerFrameUniformBufferID_global);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameUniforms), &sPerFrameUniforms, GL_STREAM_DRAW);
	printOpenGLError();

	glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	printOpenGLError();

	//draw global light
	glDisable(GL_BLEND);

	const glm::vec3 global_light_direction = pScene->GetGlobalLightDir();
	glUniform3f(glGetUniformLocation(m_uGlobalLightProgramID,"light_direction"),global_light_direction.x, global_light_direction.y, global_light_direction.z);

	glBindVertexArray(m_sFullscreenQuad.vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	printOpenGLError();

	glBindVertexArray(0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	printOpenGLError();
}

void RenderManager::DrawTransparent(Scene* pScene, Camera* pCamera)
{
	m_pFBOManager->StartScreenBufferWrite();

	GLboolean bCull = false;
	glGetBooleanv(GL_CULL_FACE, &bCull);

	glUseProgram(m_uTransparentProgramID);	
	glUniform1i(m_uDiffuseTextureLoc_Transparent,0);
	glUniform1i(m_uSpecularTextureLoc_Transparent,1);
	glUniform1i(m_uNormalTextureLoc_Transparent,2);
	printOpenGLError();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glDisable(GL_STENCIL_TEST);
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_EQUAL, 1, 0xff);
	//glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printOpenGLError();
	glDisable(GL_CULL_FACE);
	printOpenGLError();

	Matrix44 mProjectMat = *pCamera->GetProjectMat();
	Matrix44 mViewMat = *pCamera->GetViewMat();

	PerModelUniforms sPerModelUniform;
	MaterialUniforms sMaterialUniform;
	Material *pMaterial;
	uint32 modelCount = pScene->GetObjectCount();
	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	Mesh* pMesh = nullptr;
	for(uint32 i = 0; i < modelCount; ++i)
	{
		pObject = pScene->GetGameObject(i);
		pModel = pObject->GetModelComponent();
		if(pModel == nullptr || pModel->IsEnabled() == false)
			continue;

		if(pModel->IsRenderPassEnabled(RenderPass::TRANSPARENT) == false)
			continue;

		pMesh = pModel->GetMesh();
		if(pMesh == nullptr)
			continue;

		pMaterial = ResourceManager::Instance()->GetMaterial(pModel->GetMaterialIndex());
			//setup textures
		if(pMaterial->nDiffuseTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nDiffuseTextureIndex);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}
		
		if(pMaterial->nSpecularTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nSpecularTextureIndex);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}
		if(pMaterial->nNormalTextureIndex != -1)
		{
			Texture *pTexture = ResourceManager::Instance()->GetTexture(pMaterial->nNormalTextureIndex);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,pTexture->uGLID);
			printOpenGLError();
		}
		

		sMaterialUniform.vDiffuseColour = pMaterial->vDiffuseColour;
		sMaterialUniform.fShininess = pMaterial->fShininess;
		sMaterialUniform.vSpecularColour = pMaterial->vSpecularColour;
		sMaterialUniform.bMetallic = pMaterial->bMetallic;
		glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID_Transparent);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), &sMaterialUniform, GL_STREAM_DRAW);
		printOpenGLError();

		sPerModelUniform.WorldMatIn = *pObject->GetTranformMat();
		sPerModelUniform.mvpIn = mProjectMat * mViewMat * sPerModelUniform.WorldMatIn;
		sPerModelUniform.TintColor = pModel->GetTintColor();
		glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_Transparent);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), &sPerModelUniform, GL_STREAM_DRAW);
		printOpenGLError();

		glBindVertexArray(pMesh->GetVAO());
		glDrawElements(GL_TRIANGLES, pMesh->GetIndexCount() ,GL_UNSIGNED_SHORT,(void*)0);

		printOpenGLError();
	}

	if(m_bFramewire)
	{
		//currently the transparent shader is better for framewire than others
		glEnable(GL_CULL_FACE);

		glCullFace(GL_BACK);
		glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glLineWidth(1.0f); 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		Vector3 vRed(1,0,0);
		Vector3 vBlue(0,0,1);
		for(uint32 i = 0; i < modelCount; ++i)
		{
			pObject = pScene->GetGameObject(i);
			pModel = pObject->GetModelComponent();
			if(pModel == nullptr || pModel->IsEnabled() == false)
				continue;

			pMesh = pModel->GetMesh();
			if(pMesh == nullptr)
				continue;


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m_pSolidWhiteTex->uGLID);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,m_pSolidWhiteTex->uGLID);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,m_pSolidNormalTex->uGLID);
		

			sMaterialUniform.vDiffuseColour = vRed;
			sMaterialUniform.fShininess = 1;
			sMaterialUniform.vSpecularColour = vRed;
			sMaterialUniform.bMetallic = 0.0f;
			glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID_Transparent);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), &sMaterialUniform, GL_STREAM_DRAW);
			printOpenGLError();

			sPerModelUniform.WorldMatIn = *pObject->GetTranformMat();
			sPerModelUniform.mvpIn = mProjectMat * mViewMat * sPerModelUniform.WorldMatIn;
			sPerModelUniform.TintColor = pModel->GetTintColor();
			glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_Transparent);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), &sPerModelUniform, GL_STREAM_DRAW);
			printOpenGLError();

			glBindVertexArray(pMesh->GetVAO());
			glDrawElements(GL_TRIANGLES, pMesh->GetIndexCount() ,GL_UNSIGNED_SHORT,(void*)0);

			printOpenGLError();
		}

		if(m_bBackFramewire)
		{
			glCullFace(GL_FRONT);
			glDepthFunc(GL_GREATER);
			for(uint32 i = 0; i < modelCount; ++i)
			{
				pObject = pScene->GetGameObject(i);
				pModel = pObject->GetModelComponent();
				if(pModel == nullptr || pModel->IsEnabled() == false)
					continue;

				pMesh = pModel->GetMesh();
				if(pMesh == nullptr)
					continue;


				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,m_pSolidWhiteTex->uGLID);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D,m_pSolidWhiteTex->uGLID);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D,m_pSolidNormalTex->uGLID);
		

				sMaterialUniform.vDiffuseColour = vBlue;
				sMaterialUniform.fShininess = 1;
				sMaterialUniform.vSpecularColour = vBlue;
				sMaterialUniform.bMetallic = 0.0f;
				glBindBuffer(GL_UNIFORM_BUFFER, m_uMaterialUniformBufferID_Transparent);
				glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialUniforms), &sMaterialUniform, GL_STREAM_DRAW);
				printOpenGLError();

				sPerModelUniform.WorldMatIn = *pObject->GetTranformMat();
				sPerModelUniform.mvpIn = mProjectMat * mViewMat * sPerModelUniform.WorldMatIn;
				sPerModelUniform.TintColor = pModel->GetTintColor();
				glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_Transparent);
				glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), &sPerModelUniform, GL_STREAM_DRAW);
				printOpenGLError();

				glBindVertexArray(pMesh->GetVAO());
				glDrawElements(GL_TRIANGLES, pMesh->GetIndexCount() ,GL_UNSIGNED_SHORT,(void*)0);

				printOpenGLError();
			}
		}

		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthFunc(GL_LESS);
	}

	//set some important things back
	if(bCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	printOpenGLError();
}

void 
RenderManager::DrawTouchHighlight(Scene* pScene, Camera* pCamera)
{
	GameObject* pBladeObject = pScene->GetGameObject("realBlade");
	if(pBladeObject == nullptr)
		return;

	m_pFBOManager->StartScreenBufferWrite();

	glUseProgram(m_uTouchHighlightProgramID);	
	glUniform1i(m_uGBufferPosTexLoc_TouchHighlight,0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE,m_pFBOManager->GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE_POSITION));
	printOpenGLError();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_STENCIL_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

	
	TouchHighlightUniforms sTouchHighlightUniform;
	sTouchHighlightUniform.color = Vector4(0.0f,1.0f,0.0f,1.0f);
	sTouchHighlightUniform.touchRange = 0.01f;
	sTouchHighlightUniform.planeN = MFD_Normalize(pBladeObject->GetWorldRight());
	sTouchHighlightUniform.planeD =  MFD_Dot(sTouchHighlightUniform.planeN,pBladeObject->GetWorldPos());
	glBindBuffer(GL_UNIFORM_BUFFER, m_uTouchHighlightUniformBufferID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(TouchHighlightUniforms), &sTouchHighlightUniform, GL_STREAM_DRAW);
	printOpenGLError();

	Matrix44 mProjectMat = *pCamera->GetProjectMat();
	Matrix44 mViewMat = *pCamera->GetViewMat();

	PerModelUniforms sPerModelUniform;
	uint32 modelCount = pScene->GetObjectCount();
	GameObject* pObject = nullptr;
	Model* pModel = nullptr;
	Mesh* pMesh = nullptr;
	for(uint32 i = 0; i < modelCount; ++i)
	{
		pObject = pScene->GetGameObject(i);
		pModel = pObject->GetModelComponent();
		if(pModel == nullptr || pModel->IsEnabled() == false)
			continue;

		if(pModel->IsRenderPassEnabled(RenderPass::TOUCH_HIGHLIGHT) == false)
			continue;

		pMesh = pModel->GetMesh();
		if(pMesh == nullptr)
			continue;

		sPerModelUniform.WorldMatIn = *pObject->GetTranformMat();
		sPerModelUniform.mvpIn = mProjectMat * mViewMat * sPerModelUniform.WorldMatIn;
		sPerModelUniform.TintColor = pModel->GetTintColor();
		glBindBuffer(GL_UNIFORM_BUFFER, m_uPerModelUniformBufferID_TouchHighlight);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), &sPerModelUniform, GL_STREAM_DRAW);
		printOpenGLError();

		glBindVertexArray(pMesh->GetVAO());
		glDrawElements(GL_TRIANGLES, pMesh->GetIndexCount() ,GL_UNSIGNED_SHORT,(void*)0);
		printOpenGLError();
	}

	//set some important things back
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	printOpenGLError();
}