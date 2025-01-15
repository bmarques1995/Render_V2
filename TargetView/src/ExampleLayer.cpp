#include "ExampleLayer.hpp"
#include <imgui/imgui.h>

ExampleLayer::ExampleLayer()
{
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate()
{
	if (m_ShowDemoWindow)
		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}
