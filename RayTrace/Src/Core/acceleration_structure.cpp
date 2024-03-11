#include "pch.h"
#include "acceleration_structure.h"

void AccelerationStructure::init(const std::vector<ModelInfo>& models, const Device& device, const CommandSystem& commandSystem)
{
	APP_LOG_INFO("Initializing acceleration structure");

	m_device = &device;
	m_commandSystem = &commandSystem;

	createBlas(models);
}

void AccelerationStructure::cleanup()
{
	APP_LOG_INFO("Destroying acceleration structure");

	// Blas
	for (uint32_t i = 0; i < m_blas.size(); i++)
	{
		APP_LOG_INFO("Destroying BLAS {}", i);
		vkDestroyAccelerationStructureKHR(m_device->getLogical(), m_blas[i].as, nullptr);
		m_blas[i].buffer.cleanup();
	}
}

void AccelerationStructure::createBlas(const std::vector<ModelInfo>& models)
{
	APP_LOG_INFO("Creating BLAS");

	uint32_t blasCount = static_cast<uint32_t>(models.size());

	// Create blas inputs
	std::vector<BlasInput> blasInputs;
	blasInputs.reserve(blasCount);
	for (const auto& model : models)
	{
		// Geometry triangle data
		VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
		triangles.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		triangles.vertexFormat             = VK_FORMAT_R32G32B32_SFLOAT;
		triangles.vertexData.deviceAddress = model.vertexAddress;
		triangles.vertexStride             = sizeof(Vertex);
		triangles.maxVertex                = model.vertexCount - 1;
		triangles.indexType                = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress  = model.indexAddress;

		// Geometry
		VkAccelerationStructureGeometryKHR geometry{};
		geometry.sType              = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.geometryType       = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometry.flags              = VK_GEOMETRY_OPAQUE_BIT_KHR;
		geometry.geometry.triangles = triangles;

		// Offset
		VkAccelerationStructureBuildRangeInfoKHR offset{};
		offset.firstVertex     = 0;
		offset.primitiveCount  = model.indexCount / 3;
		offset.primitiveOffset = 0;
		offset.transformOffset = 0;

		BlasInput input;
		input.geometry.emplace_back(geometry);
		input.buildRangeInfo.emplace_back(offset);
		blasInputs.emplace_back(input);
	}

	// Prepare build information for blas
	std::vector<BlasBuildInfo> blasBuildInfo;
	blasBuildInfo.reserve(blasCount);
	VkDeviceSize maxScratchSize = 0;
	for (const auto& blasInput : blasInputs)
	{
		// Build info
		VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
		buildInfo.sType         = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildInfo.mode          = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildInfo.flags         = blasInput.flags | VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		buildInfo.geometryCount = static_cast<uint32_t>(blasInput.geometry.size());
		buildInfo.pGeometries   = blasInput.geometry.data();

		// Find primitive counts
		std::vector<uint32_t> maxPrimCount(blasInput.buildRangeInfo.size());
		for (uint32_t i = 0; i < blasInput.buildRangeInfo.size(); i++)
			maxPrimCount[i] = blasInput.buildRangeInfo[i].primitiveCount;

		// Query for size info
		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		vkGetAccelerationStructureBuildSizesKHR(m_device->getLogical(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, maxPrimCount.data(), &sizeInfo);

		// Keep track of the largest scratch size needed
		maxScratchSize = std::max(maxScratchSize, sizeInfo.buildScratchSize);

		// Set blas build info
		BlasBuildInfo blasInfo{};
		blasInfo.buildInfo = buildInfo;
		blasInfo.sizeInfo = sizeInfo;
		blasInfo.rangeInfo = blasInput.buildRangeInfo.data();
		blasBuildInfo.emplace_back(blasInfo);
	}

	// Create scratch buffer
	Buffer::CreateInfo bufferInfo{};
	bufferInfo.device   = m_device;
	bufferInfo.dataSize = maxScratchSize;
	bufferInfo.name     = "BLAS Scratch Buffer";

	Buffer scratchBuffer = Buffer::CreateScratchBuffer(bufferInfo);
	VkDeviceAddress scratchAddress = scratchBuffer.getDeviceAddress();

	// Build blas using batching
	std::vector<uint32_t> indices;
	VkDeviceSize batchSize  = 0;
	VkDeviceSize batchLimit = 256'000'000;
	for (uint32_t i = 0; i < models.size(); i++)
	{
		// Add the blas to the batch
		indices.push_back(i);
		batchSize += blasBuildInfo[i].sizeInfo.accelerationStructureSize;

		// Check if we need to build the batch
		if (batchSize >= batchLimit || i == blasCount - 1)
		{
			// Build the batch
			VkCommandBuffer commandBuffer = m_commandSystem->beginSingleTimeCommands();
			buildBlas(commandBuffer, indices, blasBuildInfo, scratchAddress);
			m_commandSystem->endSingleTimeCommands(commandBuffer, m_device->getGraphicsQueue());

			// Reset the batch
			batchSize = 0;
			indices.clear();
		}
	}

	// Store blas
	for (auto& build : blasBuildInfo)
		m_blas.emplace_back(build.as);

	// Cleanup
	scratchBuffer.cleanup();
}

void AccelerationStructure::buildBlas(VkCommandBuffer commandBuffer, const std::vector<uint32_t>& indices, std::vector<BlasBuildInfo>& buildInfos, VkDeviceAddress scratchAddress)
{
	for (const auto& i : indices)
	{
		APP_LOG_INFO("Building BLAS {}", i);
		
		// Create the buffer for the acceleration structure
		Buffer::CreateInfo bufferInfo{};
		bufferInfo.device   = m_device;
		bufferInfo.dataSize = buildInfos[i].sizeInfo.accelerationStructureSize;

		char name[128];
		sprintf(name, "BLAS Buffer %d", i);
		bufferInfo.name = name;

		buildInfos[i].as.buffer = Buffer::CreateAccelerationStructureBuffer(bufferInfo);

		// Create acceleration structure
		VkAccelerationStructureCreateInfoKHR createInfo{};
		createInfo.sType  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.type   = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		createInfo.size   = buildInfos[i].sizeInfo.accelerationStructureSize;
		createInfo.buffer = buildInfos[i].as.buffer.getBuffer();

		if (vkCreateAccelerationStructureKHR(m_device->getLogical(), &createInfo, nullptr, &buildInfos[i].as.as) != VK_SUCCESS)
		{
			APP_LOG_CRITICAL("Failed to create acceleration structure");
			throw std::exception();
		}

		buildInfos[i].buildInfo.dstAccelerationStructure  = buildInfos[i].as.as;
		buildInfos[i].buildInfo.scratchData.deviceAddress = scratchAddress;

		// Build the blas
		vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildInfos[i].buildInfo, &buildInfos[i].rangeInfo);

		// Set barrier to make sure the current build finishes before the next one starts
		VkMemoryBarrier barrier{};
		barrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		vkCmdPipelineBarrier(
			commandBuffer,                                          
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, // srcStageMask
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, // dstStageMask
			0, 1, &barrier, 0, nullptr, 0, nullptr);
	}
}
