#include "Texture.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12Texture.hpp"
#endif
#include "VKTexture.hpp"
#include "Image.hpp"

SampleRenderV2::Texture2D* SampleRenderV2::Texture2D::Instantiate(const std::shared_ptr<GraphicsContext>* context, const TextureElement& specification)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Texture2D((const std::shared_ptr<D3D12Context>*)(context), specification);
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKTexture2D((const std::shared_ptr<VKContext>*)(context), specification);
	}
	default:
		break;
	}
	return nullptr;
}

SampleRenderV2::Texture2D* SampleRenderV2::Texture2D::Instantiate(const std::shared_ptr<GraphicsContext>* context, const std::string& path, uint32_t slot, uint32_t spaceSet, uint32_t heapSlot, uint32_t textureIndex)
{
	
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	std::shared_ptr<Image> texture;
	texture.reset(Image::CreateImage(path));
	TextureElement specification =
		//std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet,  TextureTensor tensor, uint32_t textureIndex, size_t depth
	{ texture, slot, heapSlot, spaceSet, TextureTensor::TENSOR_2, textureIndex };
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Texture2D((const std::shared_ptr<D3D12Context>*)(context), specification);
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKTexture2D((const std::shared_ptr<VKContext>*)(context), specification);
	}
	default:
		break;
	}
	return nullptr;
}
