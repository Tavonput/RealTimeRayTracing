#ifndef SHADE_STATE_GLSL
#define SHADE_STATE_GLSL 1

#include "structures.glsl"

void sampleTextures(Material mat, int txtOffset, vec2 txtCoord, inout vec4 albedo, inout vec3 normal, inout float metallic, inout float roughness)
{
	// Note: Textures must be sourced in the same order they were added during loading

	int txtID = txtOffset + mat.textureID;

	// Albedo
	if ((mat.textureMask & ALBEDO_BIT) == ALBEDO_BIT)
	{
		vec4 albedoTxt = texture(textureSamplers[nonuniformEXT(txtID)], txtCoord);
		albedo = albedoTxt;
	}

	// Normal
	if ((mat.textureMask & NORMAL_BIT) == NORMAL_BIT)
	{
		txtID++;
		normal = texture(textureSamplers[nonuniformEXT(txtID)], txtCoord).xyz;
		normal = normal * 2.0 - 1.0;
	}

	// Alpha
	if ((mat.textureMask & ALPHA_BIT) == ALPHA_BIT)
	{
		txtID++;
		albedo.a = texture(textureSamplers[nonuniformEXT(txtID)], txtCoord).x;
	}

	// Metal
	if ((mat.textureMask & METAL_BIT) == METAL_BIT)
	{
		txtID++;
		metallic = texture(textureSamplers[nonuniformEXT(txtID)], txtCoord).x;
	}

	// Roughness
	if ((mat.textureMask & ROUGH_BIT) == ROUGH_BIT)
	{
		txtID++;
		roughness = texture(textureSamplers[nonuniformEXT(txtID)], txtCoord).x;
	}
}

#endif