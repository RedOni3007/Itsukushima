#version 330

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_diffuse;
uniform sampler2DRect sampler_specular;


layout (std140) uniform PerFrameUniforms 
{
	vec3	vCameraPos;
	float	fPadding1;
	vec3	vCameraDir;
	float	fPadding2;
};

uniform vec3 light_direction;
uniform float light_intensity = 1.0;

out vec4 finalColor;

void main(void)
{
	vec3 texel_Pos =  texelFetch(sampler_world_position, ivec2(gl_FragCoord.xy)).rgb;
	vec3 texel_Normal = texelFetch(sampler_world_normal, ivec2(gl_FragCoord.xy)).rgb;
	vec4 texel_diffuse =  texelFetch(sampler_diffuse, ivec2(gl_FragCoord.xy)).rgba;
	vec3 texel_specular = texelFetch(sampler_specular, ivec2(gl_FragCoord.xy)).rgb;

	float fShininess = texel_diffuse.a;

	vec3 N = normalize(texel_Normal);
	vec3 L = normalize(light_direction);

	float diffuseFactor = dot(L,N);

	vec3 V = normalize(vCameraPos - texel_Pos);
	vec3 RL = reflect(-L, N);

	vec3 DiffuseColor = texel_diffuse.rgb * clamp(diffuseFactor,0.0,1.0);
	vec3 SpecularColor = texel_specular * pow(clamp(dot(V, RL),0.0,1.0), fShininess);
	
	finalColor = vec4((DiffuseColor + SpecularColor) * light_intensity, 1.0f);

	//finalColor = vec4(texel_Pos, 1.0f);
	//finalColor = vec4(texel_Normal, 1.0f);
	//finalColor = vec4(texel_diffuse);
	//finalColor = vec4(texel_specular,1.0f);

}