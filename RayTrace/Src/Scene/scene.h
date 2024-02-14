#pragma once

#include "Core/device.h"
#include "Core/command.h"
#include "Core/renderer.h"

class Scene
{
public:
	virtual ~Scene() {}

	virtual void onLoad(const Device& device, const CommandSystem& commandSystem) = 0;

	virtual void onUpdate(Renderer& renderer) = 0;

	virtual void onUnload() = 0;
};