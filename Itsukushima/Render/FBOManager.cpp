#include "FBOManager.h"
#include "OpenGL_ErrorCheck.h"

#include <string>
#include <assert.h>

FBOManager::FBOManager()
{
	m_uGBufferFBO = 0;
	m_uScreenBufferFBO = 0;
	m_uDepthTextureID = 0;
	memset(&m_textureIDs[0],0,sizeof(GLuint) * GBUFFER_NUM_TEXTURES);

	m_uCurrentScreenTex = 0;
	memset(&m_ScreenFBO_Texs[0],0,sizeof(GLuint) * 2);

	m_uGBufferProgramID = 0;

	for(int32 i = 0; i < GBUFFER_NUM_TEXTURES; ++i)
	{
		m_textureShaderLoc[i] = i;
	}
}


FBOManager::~FBOManager()
{
	Release();
}

void 
FBOManager::Release()
{
	if (m_uGBufferFBO != 0) 
	{
        glDeleteFramebuffers(1, &m_uGBufferFBO);
    }

	if(m_uScreenBufferFBO != 0)
	{
		glDeleteFramebuffers(1,&m_uScreenBufferFBO);
	}

	if (m_uDepthTextureID != 0) 
	{
		glDeleteTextures(1, &m_uDepthTextureID);
	}

	if (m_textureIDs[0] != 0) 
	{
        glDeleteTextures(GBUFFER_NUM_TEXTURES, m_textureIDs);
    }

	if (m_ScreenFBO_Texs[0] != 0) 
	{
        glDeleteTextures(2, m_ScreenFBO_Texs);
    }

	m_uGBufferFBO = 0;
	m_uDepthTextureID = 0;
	memset(&m_textureIDs[0],0,sizeof(GLuint) * GBUFFER_NUM_TEXTURES);

	memset(&m_ScreenFBO_Texs[0],0,sizeof(GLuint) * 2);
}

int 
FBOManager::GetGBufferTextureLoc(FBOManager::GBUFFER_TEXTURE_TYPE eType)
{
	assert(eType < GBUFFER_NUM_TEXTURES
		&& eType >= 0 );

	return m_textureShaderLoc[eType];
}

GLuint	
FBOManager::GetGBufferTexture(FBOManager::GBUFFER_TEXTURE_TYPE eType)
{
	assert(eType < GBUFFER_NUM_TEXTURES
		&& eType >= 0 );

	return m_textureIDs[eType];
}

void 
FBOManager::SetGBufferProgramID(GLuint uGBufferProgramID)
{
	m_uGBufferProgramID = uGBufferProgramID;
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_POSITION, "fragment_pos");
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_NORMAL, "fragment_normal");
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_DIFFUSE, "fragment_diffuse");
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_SPECULAR, "fragment_specular");
}

void 
FBOManager::Resize(int32 nWindowWidth, int32 nWindowHeight)
{
	Release();
	Init(nWindowWidth, nWindowHeight);
	glViewport(0, 0, nWindowWidth, nWindowHeight);
}

bool 
FBOManager::Init(int32 nWindowWidth, int32 nWindowHeight)
{
    // Create the Gbuffer FBO
	glGenFramebuffers(1, &m_uGBufferFBO);    
	glBindFramebuffer(GL_FRAMEBUFFER, m_uGBufferFBO);

    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, m_textureIDs);

	//pos
	glBindTexture(GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_POSITION]);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, nWindowWidth, nWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_POSITION, GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_POSITION], 0);

	//normal
	glBindTexture(GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_NORMAL]);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, nWindowWidth, nWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_NORMAL, GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_NORMAL], 0);

	//diffuse color 3f + shinness 1f
	glBindTexture(GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_DIFFUSE]);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, nWindowWidth, nWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_DIFFUSE, GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_DIFFUSE], 0);

	//specular color
	glBindTexture(GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_SPECULAR]);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, nWindowWidth, nWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_SPECULAR, GL_TEXTURE_RECTANGLE, m_textureIDs[GBUFFER_TEXTURE_TYPE_SPECULAR], 0);

	// depth
	glGenTextures(1, &m_uDepthTextureID);
	glBindTexture(GL_TEXTURE_RECTANGLE, m_uDepthTextureID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, nWindowWidth, nWindowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, m_uDepthTextureID, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, 
						     GL_COLOR_ATTACHMENT1,
						     GL_COLOR_ATTACHMENT2,
						     GL_COLOR_ATTACHMENT3 };

    glDrawBuffers(GBUFFER_NUM_TEXTURES, DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) 
	{
        Debug::Log("FB error, status: 0x%x\n", Status);
        return false;
    }


	// Create the Gbuffer FBO
	glGenFramebuffers(1, &m_uScreenBufferFBO);    
	glBindFramebuffer(GL_FRAMEBUFFER, m_uScreenBufferFBO);

	glGenTextures(2, m_ScreenFBO_Texs);
	glBindTexture(GL_TEXTURE_RECTANGLE, m_ScreenFBO_Texs[0]);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, nWindowWidth, nWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_RECTANGLE, m_ScreenFBO_Texs[1]);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, nWindowWidth, nWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	m_uCurrentScreenTex =  m_ScreenFBO_Texs[0];

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_uCurrentScreenTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, m_uDepthTextureID, 0);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE) 
	{
		Debug::Log("FB error, status: 0x%x\n", Status);
        return false;
    }

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void 
FBOManager::SwitchScreenFBOTexure()
{
	m_ScreenFBO_Texs[0] = m_ScreenFBO_Texs[1];
	m_ScreenFBO_Texs[1] = m_uCurrentScreenTex;
	m_uCurrentScreenTex =  m_ScreenFBO_Texs[0];

	glBindFramebuffer(GL_FRAMEBUFFER, m_uScreenBufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_uCurrentScreenTex, 0);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void 
FBOManager::StartGBufferWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uGBufferFBO);
	printOpenGLError();

	glUseProgram(m_uGBufferProgramID);
	printOpenGLError();

	//do i need to call these for everytime glUseProgram called
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_POSITION, "fragment_pos");
	printOpenGLError();
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_NORMAL, "fragment_normal");
	printOpenGLError();
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_DIFFUSE, "fragment_diffuse");
	printOpenGLError();
	glBindFragDataLocation(m_uGBufferProgramID, FBOManager::GBUFFER_TEXTURE_TYPE_SPECULAR, "fragment_specular");
	printOpenGLError();

	glDisable(GL_BLEND);
	printOpenGLError();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	printOpenGLError();

	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	printOpenGLError();
}

void
FBOManager::StartScreenBufferWrite()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uScreenBufferFBO);
}

void
FBOManager::StartScreenBufferRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uScreenBufferFBO);
}

void 
FBOManager::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}


