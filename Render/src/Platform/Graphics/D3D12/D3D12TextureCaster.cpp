#ifdef RENDER_USES_WINDOWS

#include "D3D12TextureCaster.hpp"
#include <windows.h>
#include "Console.hpp"
#include <filesystem>

SampleRenderV2::D3D12TextureCaster::D3D12TextureCaster(bool genMipmaps) :
	TextureCaster()
{
	m_GenMipmaps = genMipmaps;
	m_OutputExtension = ".dds";
}

SampleRenderV2::D3D12TextureCaster::~D3D12TextureCaster()
{
}

void SampleRenderV2::D3D12TextureCaster::CallTextureCaster(const TextureInfo& info)
{
    //`./texconv -f R8G8B8A8_UNORM_SRGB -m 0 <texture_file> -o <output_dir>`
    std::string command = m_RunningDirectory + "/texconv.exe";

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi = { 0 };

    std::stringstream commandLine;

	commandLine << "-f R8G8B8A8_UNORM_SRGB ";
    if (m_GenMipmaps)
        commandLine << "-m 0 ";
    commandLine << info.Dir << "/" << info.Basename << info.InputExtension << " -o " << info.Dir << " -y";
    std::string cmdArg = commandLine.str();

    BOOL OK = CreateProcessA((char*)command.c_str(), (char*)cmdArg.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (OK)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    //// Wait for process to finish
    //WaitForSingleObject(pi.hProcess, INFINITE);

    //// Cleanup
    //CloseHandle(pi.hProcess);
    //CloseHandle(pi.hThread);

    Console::CoreLog("Process finished successfully.");
}

#endif
