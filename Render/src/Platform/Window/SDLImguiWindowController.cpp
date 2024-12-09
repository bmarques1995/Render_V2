#include "SDLImguiWindowController.hpp"
#include <imgui/imgui_impl_sdl3.h>

SampleRenderV2::SDLImguiWindowController::SDLImguiWindowController(const std::shared_ptr<SDLWindow>* window):
	m_Window(window)
{

	ImGui_ImplSDL3_InitForOther(std::any_cast<SDL_Window*>((*m_Window)->GetWindow()));
}

SampleRenderV2::SDLImguiWindowController::~SDLImguiWindowController()
{
	ImGui_ImplSDL3_Shutdown();
}

void SampleRenderV2::SDLImguiWindowController::ReceiveInput()
{
	ImGui_ImplSDL3_NewFrame();
}

