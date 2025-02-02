#include "TextureCaster.hpp"
#include "Application.hpp"
#include <filesystem>
#ifdef RENDER_USES_WINDOWS
#include "D3D12TextureCaster.hpp"
#endif
#include "VKTextureCaster.hpp"
#include "Application.hpp"

using std::filesystem::path;

SampleRenderV2::TextureCaster::TextureCaster()
{
	m_RunningDirectory = Application::GetInstance()->GetRunningDirectory();
}

void SampleRenderV2::TextureCaster::PushTexture(std::string_view filepath)
{
	path location = filepath;
	TextureInfo info;
	info.Dir = std::filesystem::absolute(location.parent_path()).string();
	std::replace(info.Dir.begin(), info.Dir.end(), '\\', '/');
	info.Basename = location.stem().string();
	info.InputExtension = location.extension().string();
	m_TexturesInput.push_back(info);
}

void SampleRenderV2::TextureCaster::SetGenMipmaps(bool genMipmaps)
{
	m_GenMipmaps = genMipmaps;
}

void SampleRenderV2::TextureCaster::CastToPlatformTextures()
{
	for (auto& texture : m_TexturesInput)
	{
		CallTextureCaster(texture);
	}
	m_TexturesInput.clear();
}

SampleRenderV2::TextureCaster* SampleRenderV2::TextureCaster::Instantiate(bool genMipmaps)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
	#ifdef RENDER_USES_WINDOWS
		case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
		{
			return new D3D12TextureCaster(genMipmaps);
		}
	#endif
		case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
		{
			return new VKTextureCaster(genMipmaps);
		}
		default:
			break;
	}
	return nullptr;
}
