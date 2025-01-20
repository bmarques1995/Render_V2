#include "ExampleLayer.hpp"
#include <imgui/imgui.h>

ExampleLayer::ExampleLayer()
{
}

void ExampleLayer::OnImGuiRender()
{
	if (m_ShowDemoWindow)
		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate()
{
}
