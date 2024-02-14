#pragma once

#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/device.h"
#include "Core/command.h"
#include "Core/renderer.h"
#include "Core/buffer.h"
#include "Core/render_pass.h"
#include "Core/pipeline.h"
#include "Core/rendering_structures.h"

class PyramidScene : public Scene
{
public:
	virtual void onLoad(const Device& device, const CommandSystem& commandSystem) override;

	virtual void onUpdate(RenderingContext& rctx) override;

	virtual void onUnload() override;

private:
	Buffer m_vertexBuffer;

	MeshPushConstants m_cameraMatrix;

	glm::mat4 m_model;
	glm::vec3 m_cameraPosition = { 1.0f, 1.0f, 4.0f };

	void updateRenderMatrix(float time, float aspectRatio);
};
