#version 330

out vec4 finalColor;

uniform sampler2DRect sampler_world_position;

layout (std140) uniform TouchHighlightUniforms
{
	vec3	planeN;
	float	planeD;
	vec4    color;
	float   touchRange;
};

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;

void main(void)
{
	vec3 gbuffer_Pos =  texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy)).rgb;	

	float distance = (dot(planeN, gbuffer_Pos) - planeD) / dot(planeN, planeN);

	if(abs(distance) < touchRange)
	{
		finalColor = color;
	}
	else
	{
		finalColor = vec4(0.0);
	}
}
