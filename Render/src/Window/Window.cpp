#include "Window.hpp"
#include "SDLWindow.hpp"

SampleRenderV2::Window* SampleRenderV2::Window::Instantiate(uint32_t width, uint32_t height, std::string_view title)
{
	return new SDLWindow(width, height, title);
}
