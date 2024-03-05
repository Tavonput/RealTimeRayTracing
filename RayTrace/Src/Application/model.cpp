#include "pch.h"
#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION // This must exist in only one cpp file
#include <tiny_obj_loader.h>

// --------------------------------------------------------------------------
// Model
//

Model::Instance Model::createInstance()
{
	return Model::Instance(glm::mat4(1.0f), m_index);
}

void Model::cleanup()
{
	APP_LOG_INFO("Destroying model ({})", m_index);

	m_vertexBuffer.cleanup();
	m_indexBuffer.cleanup();
	m_materialBuffer.cleanup();
	m_materialIndexBuffer.cleanup();
}

// --------------------------------------------------------------------------
// Object Loader
//

void ModelLoader::ObjLoader::loadObj(const std::string& filename)
{
	APP_LOG_INFO("Loading model: {}", filename);

	tinyobj::ObjReader reader;

	// Load model
	if (!reader.ParseFromFile(filename))
	{
		if (!reader.Error().empty())
			APP_LOG_ERROR("TinyObjReader: {}", reader.Error());

		APP_LOG_CRITICAL("Failed to load model: {}", filename);
		throw;
	}
	if (!reader.Warning().empty())
		APP_LOG_WARN("TinyObjReader: {}", reader.Warning());

	// Get data
	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materialsTOL = reader.GetMaterials();

	// Loop over all material
	for (const auto& material : materialsTOL)
	{
		Material mat;
		mat.ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
		mat.diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
		mat.specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
		mat.emission = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
		mat.transmittance = glm::vec3(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
		mat.dissolve = material.dissolve;
		mat.ior = material.ior;
		mat.shininess = material.shininess;
		mat.illum = material.illum;

		// Diffuse texture
		if (!material.diffuse_texname.empty())
		{
			mat.textureID = static_cast<int>(textures.size());
			textures.push_back(material.diffuse_texname);
		}

		materials.emplace_back(mat);
	}

	// Add default material is there were none
	if (materials.empty())
		materials.emplace_back(Material());

	// Loop over all shapes
	for (const auto& shape : shapes)
	{
		// Material indices
		matIndex.insert(matIndex.end(), shape.mesh.material_ids.begin(), shape.mesh.material_ids.end());

		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};

			// Vertex positions
			vertex.pos.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.pos.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.pos.z = attrib.vertices[3 * index.vertex_index + 2];

			// Normals
			if (index.normal_index >= 0)
			{
				vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
				vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
				vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
			}

			// Texture coordinates
			if (index.texcoord_index >= 0)
			{
				vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
				vertex.texCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];
			}

			// Colors
			if (!attrib.colors.empty())
			{
				vertex.color.x = attrib.colors[3 * index.vertex_index + 0];
				vertex.color.y = attrib.colors[3 * index.vertex_index + 1];
				vertex.color.z = attrib.colors[3 * index.vertex_index + 2];
			}

			vertices.push_back(vertex);
			indices.push_back(static_cast<int>(indices.size()));
		}
	}

	// Fix out of bounds material indices
	for (auto& index : matIndex)
	{
		if (index < 0 || index > materials.size())
			index = 0;
	}

	APP_LOG_TRACE("Number of materials: {}", materialsTOL.size());
	APP_LOG_TRACE("Number of shapes: {}", shapes.size());
	APP_LOG_TRACE("Number of vertices: {}", attrib.vertices.size());
	APP_LOG_TRACE("Number of indices: {}", indices.size());
	APP_LOG_TRACE("Number of textures: {}", textures.size());
}

// --------------------------------------------------------------------------
// Model Loader
//

Model ModelLoader::loadModel(const std::string& filename)
{
	// Load model
	ObjLoader loader;
	loader.loadObj(filename);

	// Convert from SRGB to linear
	for (auto& m : loader.materials)
	{
		m.ambient  = glm::pow(m.ambient, glm::vec3(2.2f));
		m.diffuse  = glm::pow(m.diffuse, glm::vec3(2.2f));
		m.specular = glm::pow(m.specular, glm::vec3(2.2f));
	}

	Model::CreateInfo modelInfo{};
	modelInfo.device     = m_device;
	modelInfo.modelIndex = m_modelCount;

	uint32_t numIndices  = static_cast<uint32_t>(loader.indices.size());
	uint32_t numVertices = static_cast<uint32_t>(loader.vertices.size());

	Buffer::CreateInfo createInfo{};
	createInfo.device        = m_device;
	createInfo.commandSystem = m_commandSystem;

	// Create vertex buffer
	char vertexName[128];
	sprintf(vertexName, "Vertex Buffer Model %d", m_modelCount);

	createInfo.name        = vertexName;
	createInfo.data        = loader.vertices.data();
	createInfo.dataSize    = sizeof(Vertex) * numVertices;
	createInfo.dataCount   = numVertices;
	modelInfo.vertexBuffer = Buffer::CreateVertexBuffer(createInfo);

	// Create index buffer
	char indexName[128];
	sprintf(indexName, "Index Buffer Model %d", m_modelCount);

	createInfo.name       = indexName;
	createInfo.data       = loader.indices.data();
	createInfo.dataSize   = sizeof(uint32_t) * numIndices;
	createInfo.dataCount  = numIndices;
	modelInfo.indexBuffer = Buffer::CreateIndexBuffer(createInfo);

	// Create material buffer
	char materialName[128];
	sprintf(materialName, "Material Storage Buffer Model %d", m_modelCount);

	createInfo.name          = materialName;
	createInfo.data          = loader.materials.data();
	createInfo.dataSize      = sizeof(Material) * loader.materials.size();
	createInfo.dataCount     = static_cast<uint32_t>(loader.materials.size());
	modelInfo.materialBuffer = Buffer::CreateStorageBuffer(createInfo);

	// Create material index buffer
	char materialIndexName[128];
	sprintf(materialIndexName, "Material Index Storage Buffer Model %d", m_modelCount);

	createInfo.name                = materialIndexName;
	createInfo.data               = loader.matIndex.data();
	createInfo.dataSize           = sizeof(int32_t) * loader.matIndex.size();
	createInfo.dataCount          = static_cast<uint32_t>(loader.matIndex.size());
	modelInfo.materialIndexBuffer = Buffer::CreateStorageBuffer(createInfo);

	// Store material buffer addresses
	MaterialDescription desc;
	desc.materialAddress      = modelInfo.materialBuffer.getDeviceAddress();
	desc.materialIndexAddress = modelInfo.materialIndexBuffer.getDeviceAddress();
	m_materialDescriptions.emplace_back(desc);

	m_modelCount++;

	return Model(modelInfo);
}
