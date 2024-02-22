struct GlobalUniform
{
	mat4 viewProjection;
	vec3 viewPosition;

	float lightIntensity;
	vec3 lightPosition;
	vec3 lightColor;
};

struct MaterialDescription
{
	uint64_t materialAddress;
	uint64_t materialIndexAddress;
};

struct Material
{
	vec3    ambient;
	float   shininess;

	vec3    diffuse;
	float   ior;

	vec3    specular;
	float   dissolve;

	vec3    transmittance;
	int     illum;

	vec3    emission;
	int     textureID;
};

struct PushConstant
{
	mat4 model;
	vec3 objectColor;
	int  objectID;
};
