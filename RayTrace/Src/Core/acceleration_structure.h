#pragma once

#include "Application/logging.h"
#include "Application/model.h"

#include "device.h"
#include "command.h"
#include "buffer.h"
#include "rendering_structures.h"

class AccelerationStructure
{
public:
	void init(
		const std::vector<ModelInfo>& models, 
		const std::vector<Model::Instance>& instances, 
		const Device& device, 
		const CommandSystem& commandSystem);

	void cleanup();

private:
	struct Accel
	{
		VkAccelerationStructureKHR as = VK_NULL_HANDLE;
		Buffer                     buffer;
	};

	struct BlasInput
	{
		std::vector<VkAccelerationStructureGeometryKHR>       geometry;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildRangeInfo;
		VkBuildAccelerationStructureFlagsKHR                  flags = 0;
	};

	struct BlasBuildInfo
	{
		VkAccelerationStructureBuildGeometryInfoKHR     buildInfo{};
		VkAccelerationStructureBuildSizesInfoKHR        sizeInfo{};
		const VkAccelerationStructureBuildRangeInfoKHR* rangeInfo;
		Accel                                           as;
	};

	const Device*        m_device        = nullptr;
	const CommandSystem* m_commandSystem = nullptr;

	std::vector<Accel> m_blas;
	Accel m_tlas;

	void createBlas(const std::vector<ModelInfo>& models);
	void createTlas(const std::vector<Model::Instance>& instances);

	void buildBlas(
		VkCommandBuffer              commandBuffer,
		const std::vector<uint32_t>& indices,
		std::vector<BlasBuildInfo>&  buildInfos,
		VkDeviceAddress              scratchAddress);

	void buildTlas(
		VkCommandBuffer commandBuffer,
		uint32_t        instanceCount,
		VkDeviceAddress instanceBufferAddress,
		Buffer&         scratchBuffer);

	VkTransformMatrixKHR transformMatrixToKHR(glm::mat4 matrix);
	VkDeviceAddress getBlasDeviceAddress(uint32_t id);
};

