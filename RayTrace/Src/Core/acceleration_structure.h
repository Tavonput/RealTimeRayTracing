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
	void init(const std::vector<ModelInfo>& models, const Device& device, const CommandSystem& commandSystem);

	void cleanup();

private:
	struct BlasInput
	{
		std::vector<VkAccelerationStructureGeometryKHR>       geometry;
		std::vector<VkAccelerationStructureBuildRangeInfoKHR> buildRangeInfo;
		VkBuildAccelerationStructureFlagsKHR                  flags = 0;
	};

	struct Accel
	{
		VkAccelerationStructureKHR as;
		Buffer                     buffer;
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

	void createBlas(const std::vector<ModelInfo>& models);
	void createTlas();

	void buildBlas(
		VkCommandBuffer              commandBuffer,
		const std::vector<uint32_t>& indices,
		std::vector<BlasBuildInfo>&  buildInfos,
		VkDeviceAddress              scratchAddress);
};

