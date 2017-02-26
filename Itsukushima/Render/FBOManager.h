/*
* Manager the FBOs
* todo: depend on RenderManager
*
* @author: Kai Yang
*/

#ifndef FBOManager_H
#define FBOManager_H

#include <Core/CoreHeaders.h>
#include <Core/GLEW_GLFW.h>

class FBOManager
{
public:
	 enum GBUFFER_TEXTURE_TYPE 
	 {
         GBUFFER_TEXTURE_TYPE_POSITION = 0,
         GBUFFER_TEXTURE_TYPE_NORMAL,
		 GBUFFER_TEXTURE_TYPE_DIFFUSE,
		 GBUFFER_TEXTURE_TYPE_SPECULAR,
         GBUFFER_NUM_TEXTURES
     };


public:
	FBOManager();
	~FBOManager();

	int		GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE eType);
	GLuint	GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE eType);

	bool Init(int32 nWindowWidth, int32 nWindowHeight);
	void Resize(int32 nWindowWidth, int32 nWindowHeight);

	void StartGBufferWrite();
	void StartScreenBufferWrite();
    void StartScreenBufferRead();

    void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);

	void SwitchScreenFBOTexure();

	void SetGBufferProgramID(GLuint uGBufferProgramID);

private:
	void Release();

private:
    GLuint	m_uGBufferFBO;
	GLuint	m_uScreenBufferFBO;//will blit to default buffer
    GLuint	m_uDepthTextureID;
	GLuint	m_textureIDs[GBUFFER_NUM_TEXTURES];

	GLuint  m_ScreenFBO_Texs[2];
	GLuint  m_uCurrentScreenTex;

	GLuint  m_uGBufferProgramID;

	int32	m_textureShaderLoc[GBUFFER_NUM_TEXTURES];
};

#endif