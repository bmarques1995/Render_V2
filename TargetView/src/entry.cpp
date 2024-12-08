#include "Application.hpp"
#include "Console.hpp"

#ifdef WIN32
#include <windows.h>
#endif



#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	SampleRenderV2::Console::Init();
	SampleRenderV2::Application* app = new SampleRenderV2::Application();
	app->Run();
	delete app;
	SampleRenderV2::Console::End();
	return 0;
}
