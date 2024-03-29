#include "pch.h"
#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION // This must exist in only one cpp file
#include <tiny_obj_loader.h>

// --------------------------------------------------------------------------
// Model
//

void Model::cleanup()
{
	APP_LOG_INFO("Destroying model ({})", m_index);

	m_vertexBuffer.cleanup();
	m_indexBuffer.cleanup();
	m_materialBuffer.cleanup();
	m_materialIndexBuffer.cleanup();

	for (auto& texture : m_textures)
		texture.cleanup();
}

// --------------------------------------------------------------------------
// Object Loader
//

void SceneBuilder::ObjLoader::loadObj(const std::string& filename)
{
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
		mat.ambient       = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
		mat.diffuse       = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
		mat.specular      = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
		mat.emission      = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
		mat.transmittance = glm::vec3(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
		mat.dissolve      = material.dissolve;
		mat.ior           = material.ior;
		mat.shininess     = material.shininess;
		mat.illum         = material.illum;
		mat.roughness     = material.roughness;
		mat.metallic      = material.metallic;

		// Diffuse texture
		if (!material.diffuse_texname.empty())
		{
			// Set the texture ID. This is assuming every material with a texture will always have an albedo map
			mat.textureID = static_cast<int>(textures.size());

			textures.push_back(material.diffuse_texname);
			textureTypes.push_back(Texture::FileType::ALBEDO);
			mat.textureMask |= 0x00000001; // Bit 1
		}

		if (!material.normal_texname.empty())
		{
			textures.push_back(material.normal_texname);
			textureTypes.push_back(Texture::FileType::NORMAL);
			mat.textureMask |= 0x00000002; // Bit 2
		}

		if (!material.alpha_texname.empty())
		{
			textures.push_back(material.alpha_texname);
			textureTypes.push_back(Texture::FileType::ALPHA);
			mat.textureMask |= 0x00000004; // Bit 3
		}

		if (!material.metallic_texname.empty())
		{
			textures.push_back(material.metallic_texname);
			textureTypes.push_back(Texture::FileType::METAL);
			mat.textureMask |= 0x00000008; // Bit 4
		}

		if (!material.roughness_texname.empty())
		{
			textures.push_back(material.roughness_texname);
			textureTypes.push_back(Texture::FileType::ROUGH);
			mat.textureMask |= 0x00000010; // Bit 5
		}

		materials.emplace_back(mat);
	}

	// Add default material is there were none
	if (materials.empty())
		materials.emplace_back(Material());

	// Loop over all shapes
	for (const auto& shape : shapes)
	{
		vertices.reserve(shape.mesh.indices.size() + vertices.size());
		indices.reserve(shape.mesh.indices.size() + indices.size());

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

	// Compute normal when no normal were provided
	if (attrib.normals.empty())
	{
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			Vertex& v0 = vertices[indices[i + 0]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];

			glm::vec3 n = glm::normalize(glm::cross((v1.pos - v0.pos), (v2.pos - v0.pos)));
			v0.normal = n;
			v1.normal = n;
			v2.normal = n;
		}
	}

	// Compute tangents
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		Vertex& v0 = vertices[indices[i + 0]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		glm::vec3 edge1 = v1.pos - v0.pos;
		glm::vec3 edge2 = v2.pos - v0.pos;
		glm::vec2 dUV1  = v1.texCoord - v0.texCoord;
		glm::vec2 dUV2  = v2.texCoord - v0.texCoord;

		float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

		glm::vec3 t = {
			f * (dUV2.y * edge1.x - dUV1.y * edge2.x),
			f * (dUV2.y * edge1.y - dUV1.y * edge2.y),
			f * (dUV2.y * edge1.z - dUV1.y * edge2.z),
		};

		v0.tangent = t;
		v1.tangent = t;
		v2.tangent = t;
	}

	APP_LOG_TRACE("Number of materials: {}", materialsTOL.size());
	APP_LOG_TRACE("Number of shapes: {}", shapes.size());
	APP_LOG_TRACE("Number of vertices: {}", attrib.vertices.size());
	APP_LOG_TRACE("Number of indices: {}", indices.size());
	APP_LOG_TRACE("Number of textures: {}", textures.size());
}

// --------------------------------------------------------------------------
// Scene Builder
//
void SceneBuilder::init(const Device& device, const CommandSystem& commandSystem, Gui& gui)
{
	m_device        = &device;
	m_commandSystem = &commandSystem;
	m_gui           = &gui;
}

Model SceneBuilder::loadModel(const std::string& filename)
{
	APP_LOG_INFO("Loading model {}", filename);

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

	if (m_device->isRtxSupported())
		createInfo.flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

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
	createInfo.dataCount  = numIndices;;
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
	createInfo.name               = materialIndexName;
	createInfo.data               = loader.matIndex.data();
	createInfo.dataSize           = sizeof(int32_t) * loader.matIndex.size();
	createInfo.dataCount          = static_cast<uint32_t>(loader.matIndex.size());
	modelInfo.materialIndexBuffer = Buffer::CreateStorageBuffer(createInfo);

	// Store buffer addresses
	ObjectDescription desc;
	desc.vertexAddress        = modelInfo.vertexBuffer.getDeviceAddress();
	desc.indexAddress         = modelInfo.indexBuffer.getDeviceAddress();
	desc.materialAddress      = modelInfo.materialBuffer.getDeviceAddress();
	desc.materialIndexAddress = modelInfo.materialIndexBuffer.getDeviceAddress();
	desc.textureOffset        = static_cast<uint32_t>(m_textureInfo.size());
	m_objectDescriptions.emplace_back(desc);

	// Create textures
	createTextures(loader.textures, loader.textureTypes, filename, modelInfo.textures);
	for (const auto& texture : modelInfo.textures)
		m_textureInfo.emplace_back(texture.getDescriptor());

	// Store model info
	m_modelInfos.emplace_back(m_modelCount, numVertices, numIndices, desc.vertexAddress, desc.indexAddress);
	m_modelCount++;

	return Model(modelInfo);
}

Model::Instance SceneBuilder::createInstance(const Model& model, glm::mat4 transform)
{
	Model::Instance instance;
	instance.transform = transform;
	instance.objectID  = model.getIndex();

	m_instances.emplace_back(instance);

	return instance;
}

void SceneBuilder::createTextures(const std::vector<std::string>& texturePaths, const std::vector<Texture::FileType>& textureTypes, const std::string& objPath, std::vector<Texture>& textures)
{
	// We need to have at least one texture so that the pipeline does not complain. So we create a
	// dummy texture if there are currently no textures to load and no previous textures loaded
	if (texturePaths.empty() && m_textureInfo.empty())
	{
		Texture::CreateInfo textureInfo{};
		textureInfo.pDevice        = m_device;
		textureInfo.pCommandSystem = m_commandSystem;
		textureInfo.extent         = {1, 1};
		textureInfo.name           = "Dummy Texture";

		std::vector<Texture> texture; 
		texture.emplace_back(Texture::Create(textureInfo));
		textures = std::move(texture);
		return;
	}

	// There are no textures to load
	if (texturePaths.empty())
		return;

	// Find the root path from the obj path
	size_t      lastSlash = objPath.find_last_of("/\\");
	std::string rootPath  = objPath.substr(0, lastSlash);

	std::vector<Texture::CreateInfo> infos(texturePaths.size());
	std::vector<std::stringstream>   filenames(texturePaths.size());

	// Fill out create infos. Name and filename are allocate on the heap so that they can exists outside
	// of this for loop.
	for (uint32_t i = 0; i < texturePaths.size(); i++)
	{
		char* name = new char[128];
		sprintf(name, "Texture %d Model %d", i, m_modelCount);
		infos[i].name = name;

		filenames[i] << rootPath << "/" << texturePaths[i];
		char* filename = new char[filenames[i].str().length() + 1];
		strcpy(filename, filenames[i].str().c_str());
		infos[i].filename = filename;

		infos[i].fileType = textureTypes[i];

		infos[i].pDevice        = m_device;
		infos[i].pCommandSystem = m_commandSystem;
	}

	// Create textures
	textures = std::move(Texture::CreateBatch(infos, infos.size()));

	// Free names and filenames
	for (auto& info : infos)
	{
		delete[] info.name;
		delete[] info.filename;
	}
}
