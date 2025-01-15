#pragma once

#include "Layer.hpp"

class ExampleLayer : public SampleRenderV2::Layer
{
public:
	ExampleLayer();
	~ExampleLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
private:
	bool m_ShowDemoWindow = true;
};