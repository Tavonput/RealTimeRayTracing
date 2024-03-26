#define PI 3.14159265

struct GlobalUniform
{
	mat4 viewProjection;
	mat4 viewInverse;
	mat4 projInverse;

	vec3 viewPosition;
	float lightIntensity;

	vec3 lightPosition;
	vec3 lightColor;
};

struct ObjectDescription
{
	uint64_t vertexAddress;
	uint64_t indexAddress;
	uint64_t materialAddress;
	uint64_t materialIndexAddress;
	int txtOffset;
};

struct Vertex
{
	vec3 pos;
	vec3 color;
	vec3 normal;
	vec2 texCoord;
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

	float roughness;
	float metallic;
};

struct PushConstant
{
	mat4 model;
	vec3 objectColor;
	int  objectID;
};

struct PostPushConstant
{
	float exposure;
};

struct RtxPushConstant
{
	vec4 clearColor;

	int maxDepth;
	int sampleCount;
	int frame;

	float russianRoulette;
};

struct hitPayload
{
	vec3 hitValue;
	uint depth;

	vec3 attenuation;
	uint done;

	vec3 rayOrigin;
	uint seed;

	vec3 rayDir;
};

struct hitPayloadPath
{
	vec3 throughput;
	vec3 emission;

	vec3 rayOrigin;
	vec3 rayDir;

	uint depth;
	uint seed;
	uint done;
};
