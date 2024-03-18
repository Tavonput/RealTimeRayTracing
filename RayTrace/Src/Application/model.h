#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "logging.h"
#include "Gui.h"

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

	float roughness        = 1.0f;
	float metallic         = 0.0f;
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

	Buffer& getVertexBuffer() { return m_vertexBuffer; }
	Buffer& getIndexBuffer() { return m_indexBuffer; }
	uint32_t getIndex() const { return m_index; }

	void cleanup();

private:
	const Device* m_device = nullptr;

	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
	Buffer m_materialBuffer;
	Buffer m_materialIndexBuffer;

	uint32_t m_index = 0;
};

class SceneBuilder
{
public:
	SceneBuilder() = default;
	SceneBuilder(const Device& device, const CommandSystem& commandSystem, Gui& gui)
		: m_device(&device), m_commandSystem(&commandSystem), m_gui(&gui) {}

	Model loadModel(const std::string& filename);
	Model::Instance createInstance(const Model& model, glm::mat4 transform);

	void setLightPosition(glm::vec3 pos) { m_gui->setInitialLightPosition(pos); }
	void setBackgroundColor(glm::vec3 color) { m_gui->setInitialBackground(color); }
	
	void addUICheckBox(const std::string& name, bool* button) { m_gui->addCustomCheckBox(name, button); }

	const std::vector<ModelInfo>& getModelInformation() const { return m_modelInfos; }
	const std::vector<ObjectDescription>& getObjectDescriptions() const { return m_objectDescriptions; }
	const std::vector<Model::Instance>& getInstances() const { return m_instances; }

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
	std::vector<Model::Instance>   m_instances;

	uint32_t m_modelCount = 0;

	const Device*        m_device        = nullptr;
	const CommandSystem* m_commandSystem = nullptr;
	Gui*                 m_gui           = nullptr;
};
