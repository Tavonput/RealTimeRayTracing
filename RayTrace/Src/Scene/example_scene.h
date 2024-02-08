#pragma once

#include "scene.h"

#include "Core/device.h"
#include "Core/command.h"
#include "Core/renderer.h"
#include "Core/buffer.h"
#include "Core/render_pass.h"
#include "Core/pipeline.h"

class ExampleScene : public Scene
{
public:
	virtual void onLoad(const Device& device, const CommandSystem& commandSystem) override;

	virtual void onUpdate(RenderingContext& rctx) override;

	virtual void onUnload() override;
	
private:
	Buffer m_vertexBuffer;
	Buffer m_indexBuffer;
};