#version 330

layout(location = 0) in vec3 posIn;
layout(location = 1) in vec3 normalIn;
layout(location = 2) in vec2 uvIn;

layout (std140) uniform PerModelUniforms 
{
	mat4	mvpIn;
	mat4	WorldMatIn;
	vec4    tintColor;
};

out vec3 vWorldNormal;
out vec3 vWorldPos;
out vec2 vTexCoord;

void main(void)
{
	gl_Position = mvpIn * vec4(posIn,1);
	vWorldNormal = (WorldMatIn * vec4(normalIn,0)).xyz;
	vWorldPos = (WorldMatIn * vec4(posIn,1)).xyz;
	vTexCoord = uvIn;
}
