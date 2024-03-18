#pragma once

#include "Application/model.h"

#include "Core/renderer.h"

class Scene
{
public:
	virtual ~Scene() {}

	virtual void onLoad(SceneBuilder& scene) = 0;

	virtual void onUpdate(Renderer& renderer) = 0;

	virtual void onUnload() = 0;
};