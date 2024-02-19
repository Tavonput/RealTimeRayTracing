#pragma once

#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/renderer.h"
#include "Core/render_pass.h"
#include "Core/pipeline.h"
#include "Core/rendering_structures.h"

#include "Application/model.h"

class CornellBoxScene : public Scene
{
public:
	virtual void onLoad(ModelLoader& modelLoader) override;

	virtual void onUpdate(Renderer& renderer) override;

	virtual void onUnload() override;

private:
	Model           m_cornellBoxModel;
	Model::Instance m_cornellBox;

	glm::vec3 m_lightColor    = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_lightPosition = { 0.0f, 2.5f, 0.0f }; // Original (0.0, 2.5, 0.0) - Glossy (0.0, 1.5, 0.0)
};