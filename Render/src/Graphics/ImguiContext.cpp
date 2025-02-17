#include "ImguiContext.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12ImguiContext.hpp"
#endif
#include "VKImguiContext.hpp"

void SampleRenderV2::ImguiContext::StartImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
}

void SampleRenderV2::ImguiContext::EndImgui()
{
	ImGui::DestroyContext();
}

void SampleRenderV2::ImguiContext::StartFrame()
{
	ImGui::NewFrame();
}

void SampleRenderV2::ImguiContext::EndFrame()
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

SampleRenderV2::ImguiContext* SampleRenderV2::ImguiContext::Instantiate(const std::shared_ptr<GraphicsContext>* graphicsContext)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12ImguiContext((const std::shared_ptr<D3D12Context>*)(graphicsContext));
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKImguiContext((const std::shared_ptr<VKContext>*)(graphicsContext));
	}
	default:
		break;
	}
	return nullptr;
}
