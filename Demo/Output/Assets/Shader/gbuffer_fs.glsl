#version 330

out vec3 fragment_pos;
out vec3 fragment_normal;
out vec4 fragment_diffuse;
out vec3 fragment_specular;

uniform sampler2D gSamplerDiffuse;
uniform sampler2D gSamplerSpecular;
uniform sampler2D gSamplerNormal;

layout (std140) uniform PerModelUniforms 
{
	mat4	mvpIn;
	mat4	worldMatIn;
	vec4    tintColor;
};

layout (std140) uniform MaterialUniforms 
{
	vec3	vDiffuseColour;
	float	fShininess;
	vec3	vSpecularColour;
    float	bMetallic;//0 or 1
};

uniform vec3 vCameraPos;

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;

//From http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame(vec3 N, vec3 p)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( vTexCoord );
    vec2 duv2 = dFdy( vTexCoord );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

void main(void)
{
	vec3 texDiffuse = texture(gSamplerDiffuse, vTexCoord).xyz;
	vec3 texSpecular = texture(gSamplerSpecular, vTexCoord).xyz;	
	vec3 texNormal = texture(gSamplerNormal, vTexCoord ).xyz;
	texNormal = texNormal * 2.0 - 1.0;

	vec3 V = normalize(vCameraPos - vWorldPos);
    mat3 TBN = cotangent_frame(normalize(vWorldNormal), -V);

	texDiffuse = texDiffuse * vDiffuseColour * tintColor.rgb;

	fragment_pos = vWorldPos;
	fragment_normal = normalize(TBN * texNormal);
	fragment_diffuse = vec4(texDiffuse.r, texDiffuse.g, texDiffuse.b,fShininess);
	fragment_specular = texSpecular * vSpecularColour * tintColor.rgb;
}
