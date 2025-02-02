#pragma once 

#include "TextureCaster.hpp"

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND VKTextureCaster : public TextureCaster
	{
	public:
		VKTextureCaster(bool genMipmaps);
		~VKTextureCaster();

	protected:

		void CallTextureCaster(const TextureInfo& info) override;
	};
}
