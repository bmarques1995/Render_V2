#pragma once

#include "Texture.hpp"
#include "VKContext.hpp"
#include <ktxvulkan.h>
#include <ktx.h>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND VKTexture2D : public Texture2D
	{
	public:
		VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification);
		VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification, std::string native_texture_path);
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
		
		void CreateKTXBuffer(std::string native_texture_path, ktxTexture2** kTextureBuffer);
		void CreateKTXResourceRaw(ktxTexture2* rawTexture);
		void CopyKTXBufferRaw(ktxTexture2* rawTexture);
		void UpdateTextureInfo(const ktxTexture2& kTexture);

		void TransitionImageLayout(
			VkCommandBuffer commandBuffer,
			VkImage image,
			VkFormat format,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t mipLevels);

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static VkImageType GetNativeTensor(TextureTensor tensor);
		static VkImageViewType GetNativeTensorView(TextureTensor tensor);

		const std::shared_ptr<VKContext>* m_Context;
		bool m_Loaded;
		bool m_UsingKTX;

		TextureElement m_Specification;

		ktxVulkanTexture m_KTXBuffer;

		VkImage m_Resource;
		VkDeviceMemory m_Memory;
		VkImageView m_ResourceView;
	};
}