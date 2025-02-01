#pragma once

#include <cstdint>
#include <string>
#include "GraphicsContext.hpp"
#include "TextureLayout.hpp"

namespace SampleRenderV2
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual const TextureElement& GetTextureDescription() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool IsLoaded() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		static Texture2D* Instantiate(const std::shared_ptr<GraphicsContext>* context, const TextureElement& specification);
		// For Vulkan, I'm using the location mapping slot and spaceSet(std140(slot, spaceSet)) to locate the texture
		// For D3D12, I'm using the heapSlot(RootSignature descriptor range index) and textureIndex(a descriptor table can handle multiple textures)
		// In the moment that I add shaders preprocessing, I will set these parameters to the shader result
		static Texture2D* Instantiate(const std::shared_ptr<GraphicsContext>* context, const std::string& path, uint32_t slot, uint32_t spaceSet, uint32_t heapSlot, uint32_t textureIndex);
	};
}
