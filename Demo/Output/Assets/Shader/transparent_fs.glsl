#version 330

out vec4 finalColor;

uniform sampler2D gSamplerDiffuse;
uniform sampler2D gSamplerSpecular;
uniform sampler2D gSamplerNormal;

layout (std140) uniform PerModelUniforms 
{
	mat4	mvpIn;
	mat4	WorldMatIn;
	vec4    tintColor;
};

layout (std140) uniform MaterialUniforms 
{
	vec3	vDiffuseColour;
	float	fShininess;
	vec3	vSpecularColour;
    float	bMetallic;//0 or 1
};

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;

void main(void)
{
	//so far, just diffuse color, no lights
	vec4 texDiffuse = texture(gSamplerDiffuse, vTexCoord);
	vec4 DiffuseColor = texDiffuse * vec4(vDiffuseColour,1.0);
	finalColor = DiffuseColor * tintColor;

}
