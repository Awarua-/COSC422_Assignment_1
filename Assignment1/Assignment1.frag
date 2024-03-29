// shadertype=glsl
#version 330

uniform sampler2D water;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform int wireframe;

in float diffuse;
in vec2 texCoord;
in vec4 texWeight;
out vec4 FragColor;

void main() 
{
	vec4 texWater = texture(water, texCoord);
	vec4 texGrass = texture(grass, texCoord);
	vec4 texRock = texture(rock, texCoord);
	vec4 texSnow = texture(snow, texCoord);

	vec4 diffuseVec = vec4(diffuse, diffuse, diffuse, 1);

	if (wireframe == 1) {
		FragColor = vec4(0, 1, 0, 1) * diffuseVec;
		return;
	}
	
	FragColor = (texWater * texWeight.x + texGrass * texWeight.y + texRock * texWeight.z + texSnow * texWeight.w) * diffuse;
}