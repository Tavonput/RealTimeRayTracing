#include "test_header.h"

// -------------------------------------------------------------------------------------------------------------
// 
// Unit testing the classes that are present in the Core directory of RayTrace
// 
// -------------------------------------------------------------------------------------------------------------

namespace CoreTest
{
	// ---------------------------------------------------------------------------------------------------------
	// Render Pass
	//
	TEST_CLASS(RenderPassTest)
	{
	public:
		TEST_METHOD(BuilderCreation)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			Assert::IsNotNull(builder.m_device);
		}

		TEST_METHOD(AddOneColorAttachment)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			Assert::IsTrue(builder.m_usingColor);
			Assert::IsTrue(builder.m_colorAttachments.size() == 1);
		}

		TEST_METHOD(AddMultipleColorAttachments)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			Assert::IsTrue(builder.m_colorAttachments.size() == 2);
			Assert::IsTrue(builder.m_colorAttachmentRefs[0].attachment == 0);
			Assert::IsTrue(builder.m_colorAttachmentRefs[1].attachment == 1);
		}

		TEST_METHOD(AddOneDepthAttachment)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addDepthAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				{ { 1.0f, 1.0f } });

			Assert::IsTrue(builder.m_usingDepth);
		}

		TEST_METHOD(AddOneResolveAttachment)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addResolveAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_IMAGE_LAYOUT_UNDEFINED,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			Assert::IsTrue(builder.m_usingResolve);
		}

		TEST_METHOD(AddMixtureOfAttachments)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			builder.addDepthAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				{ { 1.0f, 1.0f } });

			builder.addResolveAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_IMAGE_LAYOUT_UNDEFINED,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			Assert::IsTrue(builder.m_attachments.size() == 4);
			Assert::IsTrue(builder.m_attachments[0].finalLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			Assert::IsTrue(builder.m_attachments[1].finalLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			Assert::IsTrue(builder.m_attachments[2].finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			Assert::IsTrue(builder.m_attachments[3].finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		}

		TEST_METHOD(ResetBuilder)
		{
			Device dummyDevice;
			RenderPass::Builder builder(dummyDevice);

			builder.addColorAttachment(
				VK_FORMAT_R8G8B8_UNORM,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				{ { 1.0f, 1.0f, 1.0f, 1.0f } });

			builder.reset();

			Assert::IsFalse(builder.m_usingColor);
		}
	};

	// ---------------------------------------------------------------------------------------------------------
	// Pipeline
	//
	TEST_CLASS(PipelineTest)
	{
		TEST_METHOD(EnableMultiSampling)
		{
			Device dummyDevice;
			Pipeline::Builder builder(dummyDevice);

			builder.addGraphicsBase();
			builder.enableMultisampling(VK_SAMPLE_COUNT_4_BIT);

			Assert::IsTrue(builder.m_multisampling.rasterizationSamples & VK_SAMPLE_COUNT_4_BIT);
		}
	};

	// ---------------------------------------------------------------------------------------------------------
	// Buffer
	//
	TEST_CLASS(BufferTest)
	{
		TEST_METHOD_INITIALIZE(Initialize)
		{
			m_window.init(100, 100);
			m_context.init(m_window);
			m_commandSystem.init(m_context.getDevice(), 2);
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			m_commandSystem.cleanup();
			m_context.cleanup();
			m_window.cleanup();
		}

		TEST_METHOD(BufferCount)
		{
			std::vector<uint32_t> data = {
				1, 2, 3, 4
			};

			Buffer::CreateInfo info;
			info.device = &m_context.getDevice();
			info.commandSystem = &m_commandSystem;
			info.data = data.data();
			info.dataSize = sizeof(uint32_t) * data.size();
			info.dataCount = static_cast<uint32_t>(data.size());

			Buffer buffer = Buffer::CreateIndexBuffer(info);

			Assert::IsTrue(buffer.getCount() == data.size());

			buffer.cleanup();
		}

	private:
		Window        m_window;
		SystemContext m_context;
		CommandSystem m_commandSystem;
	};

	// ---------------------------------------------------------------------------------------------------------
	// Descriptor Set
	//
	TEST_CLASS(DescriptorSetTest)
	{
	public:
		TEST_METHOD(DifferentBufferWrites)
		{
			DescriptorSet dummySet(VK_NULL_HANDLE, VK_NULL_HANDLE);

			dummySet.addBufferWrite(Buffer(), BufferType::UNIFORM, 0, 0);
			Assert::IsTrue(dummySet.m_descriptorWrites[0].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

			dummySet.addBufferWrite(Buffer(), BufferType::STORAGE, 0, 0);
			Assert::IsTrue(dummySet.m_descriptorWrites[1].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

			Assert::ExpectException<std::exception>([&dummySet]
				{
					dummySet.addBufferWrite(Buffer(), BufferType::VERTEX, 0, 0);
				});
		}
	};
	TEST_CLASS(DescriptorPoolTest)
	{
	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			m_window.init(100, 100);
			m_context.init(m_window);
			m_commandSystem.init(m_context.getDevice(), 2);
		}
		TEST_METHOD_CLEANUP(Cleanup)
		{
			m_commandSystem.cleanup();
			m_context.cleanup();
			m_window.cleanup();
		}
		TEST_METHOD(DescriptorPoolInit) //Acceptance test
		{
			DescriptorPool dummyPool;
			dummyPool.init(m_context.getDevice(), 2, 2);
			Assert::IsTrue(dummyPool.m_pool != VK_NULL_HANDLE);
			Assert::IsTrue(dummyPool.m_imguiDescPool != VK_NULL_HANDLE);
			


		}
		TEST_METHOD(GetImguiPool) //Acceptance test
		{
			DescriptorPool dummyPool;
			dummyPool.init(m_context.getDevice(), 2, 2);
			Assert::IsTrue(dummyPool.m_imguiDescPool == dummyPool.getImguiPool());
		}
	private:
		Window        m_window;
		SystemContext m_context;
		CommandSystem m_commandSystem;
	};
}
