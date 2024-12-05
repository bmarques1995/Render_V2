#include "SDLWindow.hpp"
#include <cassert>
#include <windows.h>

SampleRenderV2::SDLWindow::SDLWindow(uint32_t width, uint32_t height, std::string_view title) :
	m_Width(width), m_Height(height), m_Title(title), m_Minimized(false), m_ShouldClose(false), m_FullScreen(false)
{
	int result;
	m_Resizer = nullptr;
	result = SDL_Init(SDL_INIT_EVENTS);
	assert(result);

	Uint32 window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	m_Window = SDL_CreateWindow(m_Title.c_str(), width, height, window_flags);
	assert(m_Window != nullptr);

	SDL_ShowWindow(m_Window);
}

SampleRenderV2::SDLWindow::~SDLWindow()
{
	SDL_DestroyWindow(m_Window);
}

uint32_t SampleRenderV2::SDLWindow::GetWidth() const
{
	return m_Width;
}

uint32_t SampleRenderV2::SDLWindow::GetHeight() const
{
	return m_Height;
}

std::any SampleRenderV2::SDLWindow::GetNativePointer() const
{
#ifdef RENDER_USES_WINDOWS
	return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
	return nullptr;
#endif
}

std::any SampleRenderV2::SDLWindow::GetInstance() const
{
#ifdef RENDER_USES_WINDOWS
	return (HINSTANCE)SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, NULL);
#else
	return nullptr;
#endif
}

void SampleRenderV2::SDLWindow::ResetTitle(std::string newTitle)
{
	m_Title = newTitle;
	SDL_SetWindowTitle(m_Window, m_Title.c_str());
}

bool SampleRenderV2::SDLWindow::ShouldClose() const
{
	return m_ShouldClose;
}

const bool* SampleRenderV2::SDLWindow::TrackWindowClosing() const
{
	return &m_ShouldClose;
}

bool SampleRenderV2::SDLWindow::IsMinimized() const
{
	return m_Minimized;
}

void SampleRenderV2::SDLWindow::Update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
			m_Minimized = true;
		if (event.type == SDL_EVENT_WINDOW_RESTORED)
			m_Minimized = false;
		if ((event.type == SDL_EVENT_WINDOW_RESIZED) && (!m_Minimized) && m_Resizer)
			m_Resizer(event.window.data1, event.window.data2);
		if (event.type == SDL_EVENT_QUIT)
			m_ShouldClose = true;
		if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_Window))
			m_ShouldClose = true;
		if (event.type == SDL_EVENT_KEY_DOWN)
		{
			if (event.key.key == SDLK_SPACE)
			{
				m_FullScreen = !m_FullScreen;
				SDL_SetWindowFullscreen(m_Window, m_FullScreen);
			}
		}
	}
}

void SampleRenderV2::SDLWindow::ConnectResizer(std::function<void(uint32_t, uint32_t)> resizer)
{
	m_Resizer = resizer;
}