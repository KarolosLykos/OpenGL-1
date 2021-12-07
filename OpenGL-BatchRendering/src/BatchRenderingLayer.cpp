#include "BatchRenderingLayer.h"

using namespace GLCore;
using namespace GLCore::Utils;

BatchRenderingLayer::BatchRenderingLayer()
{
}

BatchRenderingLayer::~BatchRenderingLayer()
{
}

void BatchRenderingLayer::OnAttach()
{
	EnableGLDebugging();

	// Init here
}

void BatchRenderingLayer::OnDetach()
{
	// Shutdown here
}

void BatchRenderingLayer::OnEvent(Event& event)
{
	// Events here
}

void BatchRenderingLayer::OnUpdate(Timestep ts)
{
	// Render here
}

void BatchRenderingLayer::OnImGuiRender()
{
	// ImGui here
}
