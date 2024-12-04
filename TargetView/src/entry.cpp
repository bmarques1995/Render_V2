#include "Render.hpp"

#ifdef WIN32
#include <windows.h>
#endif



#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	SampleRenderV2::Sample::Hello();
	return 0;
}
