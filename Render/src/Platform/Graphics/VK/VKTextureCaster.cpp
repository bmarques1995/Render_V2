#include "VKTextureCaster.hpp"
#include <filesystem>
#include "Console.hpp"
#ifdef RENDER_USES_WINDOWS
#include <windows.h>
#endif

SampleRenderV2::VKTextureCaster::VKTextureCaster(bool genMipmaps) :
	TextureCaster()
{
	m_GenMipmaps = genMipmaps;
	m_OutputExtension = ".ktx2";
}

SampleRenderV2::VKTextureCaster::~VKTextureCaster()
{
}

void SampleRenderV2::VKTextureCaster::CallTextureCaster(const TextureInfo& info)
{
#ifdef RENDER_USES_WINDOWS
	std::string command = m_RunningDirectory + "/toktx.exe";

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi = { 0 };

    std::stringstream commandLine;

    //--genmipmap --encode uastc .\assets\textures\yor.ktx2 .\assets\textures\yor.png

    commandLine << "--encode uastc ";
    if (m_GenMipmaps)
        commandLine << "--genmipmap --filter box ";
    commandLine << info.Dir << "/" << info.Basename << m_OutputExtension << " " << info.Dir << "/" << info.Basename << info.InputExtension;
    std::string cmdArg = commandLine.str();

    // Create the process
    BOOL OK = CreateProcessA((char*)command.c_str(), (char*)cmdArg.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (OK)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        Console::CoreError("Process finished successfully.");
    }
#endif
}
