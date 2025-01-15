#include "AppContext.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	AppContext* app = new AppContext();
	app->Run();
	delete app;
	return 0;
}
