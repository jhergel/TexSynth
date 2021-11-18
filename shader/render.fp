#version 400

uniform sampler2D ctexture;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

#define M_PI 3.1415926535897932384626433832795

void main()
{

	vec2 uv = texCoord.xy;
	vec4 c = texture2D(ctexture, uv);
	FragColor = c;
	
}
