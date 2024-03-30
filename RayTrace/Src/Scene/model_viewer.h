#pragma once

#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/renderer.h"
#include "Core/render_pass.h"
#include "Core/pipeline.h"
#include "Core/rendering_structures.h"

#include "Application/model.h"

class ModelViewerScene : public Scene
{
public:
	virtual void onLoad(SceneBuilder& scene) override;

	virtual void onUpdate(Renderer& renderer) override;

	virtual void onUnload() override;

private:
	Model           m_mainModel;
	Model::Instance m_model;

	bool m_visualizeLight = false;
};
