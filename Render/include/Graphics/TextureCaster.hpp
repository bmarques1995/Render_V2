#pragma once

#include <string>
#include <vector>
#include "RenderDLLMacro.hpp"

namespace SampleRenderV2
{
	struct TextureInfo
	{
		std::string Basename;
		std::string InputExtension;
		std::string Dir;
	};
	
	class SAMPLE_RENDER_DLL_COMMAND TextureCaster
	{
	public:
		TextureCaster();
		virtual ~TextureCaster() = default;

		void PushTexture(std::string_view filepath);
		void SetGenMipmaps(bool genMipmaps);
		void CastToPlatformTextures();

		static TextureCaster* Instantiate(bool genMipmaps);

	protected:
		virtual void CallTextureCaster(const TextureInfo& info) = 0;

		bool m_GenMipmaps = true;
		std::string m_OutputExtension;
		std::vector<TextureInfo> m_TexturesInput;
		std::string m_RunningDirectory;
	};
}