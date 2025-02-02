#include "AppContext.hpp"

#ifdef WINDOWED_APP
#include <windows.h>
#include <shellapi.h>
#endif


#ifdef WINDOWED_APP
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
#ifdef WINDOWED_APP
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
#ifdef WINDOWED_APP
    LocalFree(wargv);
#endif
	return 0;
}
