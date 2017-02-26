/*
* The struct defines for render manager
* These struct will be passed to opengl, so better let them stay as glm type
*
* @author: Kai Yang
*/

#ifndef RENDER_MANAGER_DEFINES_H
#define RENDER_MANAGER_DEFINES_H

#include <Core/GLEW_GLFW.h>
#include <glm/glm.hpp>

#include <Core/CoreHeaders.h>

//render passes
enum class RenderPass : int32 
{
	GBUFFER = 1<<0, 
	LIGHTS = 1<<1, 
	TRANSPARENT = 1<<2, 
	TOUCH_HIGHLIGHT = 1<<3, 
};


//uniforms
struct PerFrameUniforms 
{
	glm::vec3	vCameraPos;
	float		fPadding1;
	glm::vec3	vCameraDir;
	float		fPadding2;
};

struct MaterialUniforms 
{
	glm::vec3	vDiffuseColour;
	float		fShininess;
	glm::vec3	vSpecularColour;
    float		bMetallic;//0 or 1
};

struct PerModelUniforms 
{
	glm::mat4	mvpIn;
	glm::mat4	WorldMatIn;
	glm::vec4   TintColor;
};

struct SpotLightUniforms 
{
	glm::vec3	vLightPos;
	float		fLightRange;
	glm::vec3	vLightColor;
	float		fLightIntensity;
	glm::vec3	vLightDirect;
	float		fLightAngle;
};

struct TouchHighlightUniforms
{
	glm::vec3	planeN;
	float		planeD;
	glm::vec4   color;
	float		touchRange;
};

struct GL_Mesh
{
        GLuint vertex_vbo;
        GLuint element_vbo;
        GLuint vao;
        int element_count;

        GL_Mesh() : vertex_vbo(0),
                 element_vbo(0),
                 vao(0),
                 element_count(0) {}
};


#endif