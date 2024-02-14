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

class SimpleCubeScene : public Scene
{
public:
	virtual void onLoad(const Device& device, const CommandSystem& commandSystem) override;

	virtual void onUpdate(RenderingContext& rctx) override;

	virtual void onUnload() override;
	
private:
	Buffer m_vertexBuffer;

	// Transforms
	glm::mat4 m_mainCubeTransform;
	glm::mat4 m_lightCubeTransform;

	// Light cube properties
	glm::vec3 m_lightColor    = { 0.8f, 0.5f, 0.2f };
	glm::vec3 m_lightPosition = { 1.0f, 0.0f, 1.0f };

	float m_totalAngle = 0.0f;
	float m_radius     = 3.0f;
	float m_speed      = 1.5f;

	void updateMainCubeTransform(RenderingContext& rctx);
	void updateLightPosition(RenderingContext& rctx);
};
