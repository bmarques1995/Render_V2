#include "AppContext.hpp"

#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif


#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
#ifdef WIN32
    LPWSTR* wargv;
    int argc;

    wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) {
        MessageBox(nullptr, "Failed to parse command line", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Convert to narrow string for printing (if needed)
    for (int i = 0; i < argc; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
        std::string arg(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, arg.data(), len, nullptr, nullptr);
    }
    AppContext* app = new AppContext(argc, wargv);
#else
	AppContext* app = new AppContext(argc, argv);
#endif
	
	app->Run();
	delete app;

    // Free memory allocated by CommandLineToArgvW
    LocalFree(wargv);
	return 0;
}
