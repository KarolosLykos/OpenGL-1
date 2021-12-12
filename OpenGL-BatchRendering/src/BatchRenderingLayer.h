#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

class BatchRenderingLayer : public GLCore::Layer
{
public:
	BatchRenderingLayer();
	virtual ~BatchRenderingLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraController;

	GLuint m_QuadVA, m_QuadVB, m_QuadIB, m_Elephant, m_Lupus;

	float m_QuadOnePosition[2] = { -1.5f, -0.5f };
	float m_QuadTwoPosition[2] = {	0.5f, -0.5f };
};