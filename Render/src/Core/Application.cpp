#include "Application.hpp"

#ifdef RENDER_USES_WINDOWS
	#include <windows.h>
#endif

SampleRenderV2::Application::Application()
{
}

SampleRenderV2::Application::~Application()
{
}

void SampleRenderV2::Application::Run()
{
	OutputDebugStringA("Sample render OK\n");
}
