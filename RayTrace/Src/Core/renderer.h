#pragma once

#include "Application/logging.h"
#include "Application/camera.h"

#include "device.h"
#include "swapchain.h"
#include "command.h"
#include "render_pass.h"
#include "buffer.h"
#include "pipeline.h"
#include "rendering_structures.h"
#include "descriptor.h"

struct RenderingContext
{
	// Provided by the application
	Swapchain&     swapchain;
	CommandSystem& commandSystem;

	std::vector<RenderPass>& renderPasses;
	std::vector<Pipeline>&   pipelines;

	std::vector<Buffer>&        uniformBuffers;
	std::vector<DescriptorSet>& descriptorSets;

	Camera camera;

	uint32_t framesInFlight = 2;

	// Current rendering state
	uint32_t frameIndex = 0;
	uint32_t imageIndex = 0;

	float deltaTime     = 0.0f;
	float lastFrameTime = 0.0f;

	float aspectRatio = 0.0f;

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	Buffer vertexBuffer;
	Buffer indexBuffer;

	RenderPass::PassType   passIndex     = RenderPass::MAIN;
	Pipeline::PipelineType pipelineIndex = Pipeline::LIGHTING;

	GlobalUniform     ubo;
	MeshPushConstants pushConstants{};

	// Constructor
	RenderingContext(
		Swapchain&                  _swapchain, 
		CommandSystem&              _commandSystem, 
		std::vector<RenderPass>&    _renderPasses,
		std::vector<Pipeline>&      _pipelines,
		std::vector<Buffer>&        _uniformBuffers,
		std::vector<DescriptorSet>& _descriptorSets,
		Camera&                     _camera,
		uint32_t                    _framesInFlight
	)
		: swapchain     (_swapchain), 
		  commandSystem (_commandSystem), 
		  renderPasses  (_renderPasses),
		  pipelines     (_pipelines), 
		  uniformBuffers(_uniformBuffers),
		  descriptorSets(_descriptorSets),
		  camera        (_camera),
		  framesInFlight(_framesInFlight)
	{}
};

class Renderer
{
public:
	static void BeginFrame(RenderingContext& ctx);
	static void Submit(RenderingContext& ctx);
	static void EndFrame(RenderingContext& ctx);

	static void BeginRenderPass(RenderingContext& ctx, RenderPass::PassType pass);
	static void EndRenderPass(RenderingContext& ctx);

	static void BindPipeline(RenderingContext& ctx, Pipeline::PipelineType pipeline);
	static void BindVertexBuffer(RenderingContext& ctx, Buffer& vertexBuffer);
	static void BindIndexBuffer(RenderingContext& ctx, Buffer& indexBuffer);
	static void BindDescriptorSets(RenderingContext& ctx);
	static void BindPushConstants(RenderingContext& ctx);

	static void DrawVertex(RenderingContext& ctx);
	static void DrawIndexed(RenderingContext& ctx);
};
