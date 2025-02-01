#pragma once

#include "Texture.hpp"
#include "VKContext.hpp"

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND VKTexture2D : public Texture2D
	{
	public:
		VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification);
		~VKTexture2D();

		const TextureElement& GetTextureDescription() const override;

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		bool IsLoaded() const override;
	
		VkImage GetResource() const;
		VkDeviceMemory GetMemory() const;
		VkImageView GetView() const;

	private:
		void CreateResource();
		void CopyBuffer();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static VkImageType GetNativeTensor(TextureTensor tensor);
		static VkImageViewType GetNativeTensorView(TextureTensor tensor);

		const std::shared_ptr<VKContext>* m_Context;
		bool m_Loaded;

		TextureElement m_Specification;

		VkImage m_Resource;
		VkDeviceMemory m_Memory;
		VkImageView m_ResourceView;
	};
}