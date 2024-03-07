#pragma once

#include "CppUnitTest.h"

#include "pch.h"

// it's for testing i swear
#define private public

#include "Application/application.h"
#include "Application/logging.h"
#include "Application/camera.h"
#include "Application/window.h"
#include "Application/event.h"
#include "Application/Gui.h"
#include "Application/model.h"

#include "Core/system_context.h"
#include "Core/swapchain.h"
#include "Core/render_pass.h"
#include "Core/command.h"
#include "Core/pipeline.h"
#include "Core/shader.h"
#include "Core/buffer.h"
#include "Core/image.h"
#include "Core/depth_buffer.h"
#include "Core/renderer.h"
#include "Core/descriptor.h"
#include "Core/rendering_structures.h"
#include "Core/framebuffer.h"
#include "Core/texture.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;