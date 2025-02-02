#pragma once

#ifdef RENDER_USES_WINDOWS

#include "TextureCaster.hpp"

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND D3D12TextureCaster : public TextureCaster
	{
	public:
		D3D12TextureCaster(bool genMipmaps);
		~D3D12TextureCaster();

	protected:

		void CallTextureCaster(const TextureInfo& info) override;
	};
}

#endif