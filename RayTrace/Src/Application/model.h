#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "logging.h"

#include "Core/device.h"
#include "Core/buffer.h"
#include "Core/rendering_structures.h"
#include "Core/command.h"

struct Material
{
	glm::vec3 ambient       = { 0.1f, 0.1f, 0.1f };
	float     shininess     = 0.0;

	glm::vec3 diffuse       = { 0.5f, 0.5f, 0.5f };
	float     ior           = 1.0f;

	glm::vec3 specular      = { 1.0f, 1.0f, 1.0f };
	float     dissolve      = 1.0f;

	glm::vec3 transmittance = { 0.0f, 0.0f, 0.0f };
	int32_t   illum         = 0;

	glm::vec3 emission      = { 0.0f, 0.0f, 0.0f };
	int32_t   textureID     = -1;
};

struct ObjectDescription
{
	uint64_t vertexAddress;
	uint64_t indexAddress;
	uint64_t materialAddress;
	uint64_t materialIndexAddress;
};

struct ModelInfo
{
	uint32_t id;
	uint32_t vertexCount;
	uint32_t indexCount;
	uint64_t vertexAddress;
	uint64_t indexAddress;
};

class Model
{
public:
	// Create Info
	struct CreateInfo
	{
		Buffer vertexBuffer;
		Buffer indexBuffer;
		Buffer materialBuffer;
		Buffer materialIndexBuffer;

		uint32_t modelIndex  = 0;
		const Device* device = nullptr;
	};

	// Instance
	struct Instance
	{
		glm::mat4 transform = glm::mat4(1.0f);
		uint32_t  objectID  = 0;

		Instance() = default;

		Instance(glm::mat4 _transform, uint32_t _objectID)
			: transform(_transform), objectID(_objectID) {}
	};

	Model() = default;

	Model(Model::CreateInfo info)
		: m_vertexBuffer       (info.vertexBuffer),
		  m_indexBuffer        (info.indexBuffer),
		  m_materialBuffer     (info.materialBuffer),
		  m_materialIndexBuffer(info.materialIndexBuffer),
		  m_device             (info.device),
	      m_index              (info.modelIndex){}

	Model::Instance createInstance();

	Buffer& getVertexBuffer() { return m_vertexBuffer; }
	Buffer& getIndexBuffer() { return m_indexBuffer; }

	void cleanup();

private:
	const Device* m_device = nullptr;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	Buffer m_materialBuffer;
	Buffer m_materialIndexBuffer;

	uint32_t m_index = 0;
};

class ModelLoader
{
public:
	ModelLoader() = default;
	ModelLoader(const Device& device, const CommandSystem& commandSystem)
		: m_device(&device), m_commandSystem(&commandSystem) {}

	Model loadModel(const std::string& filename);

	std::vector<ModelInfo>& getModelInformation() { return m_modelInfos; }
	std::vector<ObjectDescription>& getObjectDescriptions() { return m_objectDescriptions; }

private:
	// Object loader
	struct ObjLoader
	{
		std::vector<Vertex>      vertices;
		std::vector<uint32_t>    indices;
		std::vector<Material>    materials;
		std::vector<int32_t>     matIndex;
		std::vector<std::string> textures;

		void loadObj(const std::string& filename);
	};

	std::vector<ModelInfo>         m_modelInfos;
	std::vector<ObjectDescription> m_objectDescriptions;

	uint32_t m_modelCount = 0;

	const Device*        m_device        = nullptr;
	const CommandSystem* m_commandSystem = nullptr;
};
