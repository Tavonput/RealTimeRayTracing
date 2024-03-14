#pragma once

#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/renderer.h"
#include "Core/render_pass.h"
#include "Core/pipeline.h"

#include "Application/model.h"

class SimpleCubeScene : public Scene
{
public:
	virtual void onLoad(SceneBuilder& scene) override;

	virtual void onUpdate(Renderer& renderer) override;

	virtual void onUnload() override;
	
private:
	Model           m_cubeModel;
	Model::Instance m_mainCube;
	Model::Instance m_lightCube;

	// Light cube properties
	glm::vec3 m_lightColor    = { 1.0f, 0.8f, 0.4f };
	glm::vec3 m_lightPosition = { 1.0f, 0.0f, 1.0f };

	float m_totalAngle = 0.0f;
	float m_radius     = 3.0f;
	float m_speed      = 1.5f;

	void updateLightCube(Renderer& renderer);
};
