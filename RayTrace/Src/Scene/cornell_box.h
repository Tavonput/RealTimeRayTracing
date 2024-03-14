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
	virtual void onLoad(SceneBuilder& scene) override;

	virtual void onUpdate(Renderer& renderer) override;

	virtual void onUnload() override;

private:
	Model           m_cornellBoxModel;
	Model::Instance m_cornellBox;

	Model           m_mirrorModel;
	Model::Instance m_leftMirror;
	Model::Instance m_rightMirror;

	bool m_renderMirrors  = true;
	bool m_visualizeLight = true;
};