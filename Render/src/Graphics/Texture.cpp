#include "Texture.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12Texture.hpp"
#endif
#include "VKTexture.hpp"
#include "Image.hpp"

SampleRenderV2::Texture2D* SampleRenderV2::Texture2D::Instantiate(const std::shared_ptr<GraphicsContext>* context, const std::string& path, uint32_t slot, uint32_t spaceSet, uint32_t heapSlot, uint32_t textureIndex)
{
	const std::string target = "[[native]]";
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::shared_ptr<Image> texture;
	size_t pos = path.find(target);
	bool isTextureNative = (pos != std::string::npos);
	if(!isTextureNative)
		texture.reset(Image::CreateImage(path));
	TextureElement specification =
		//std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet,  TextureTensor tensor, uint32_t textureIndex, size_t depth
	{ texture, slot, heapSlot, spaceSet, TextureTensor::TENSOR_2, textureIndex };
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		if (isTextureNative)
		{
			std::string native_path = path;
			std::string replacement = "dds";
			native_path.replace(pos, target.length(), replacement);
			return new D3D12Texture2D((const std::shared_ptr<D3D12Context>*)(context), specification, native_path);
		}
		else
			return new D3D12Texture2D((const std::shared_ptr<D3D12Context>*)(context), specification);
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		if (isTextureNative)
		{
			std::string native_path = path;
			std::string replacement = "ktx2";
			native_path.replace(pos, target.length(), replacement);
			return new VKTexture2D((const std::shared_ptr<VKContext>*)(context), specification, native_path);
		}
		else
			return new VKTexture2D((const std::shared_ptr<VKContext>*)(context), specification);
	}
	default:
		break;
	}
	return nullptr;
}
