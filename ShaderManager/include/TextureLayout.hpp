#pragma once

#include "Image.hpp"
#include "ShaderManagerDLLMacro.hpp"
#include <cstdint>

namespace SampleRenderV2
{
	enum class SAMPLE_SHADER_MNG_DLL_COMMAND TextureTensor
	{
		TENSOR_1 = 1,
		TENSOR_2,
		TENSOR_3
	};

	class SAMPLE_SHADER_MNG_DLL_COMMAND TextureElement
	{
	public:
		TextureElement();
		TextureElement(std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t samplerRegister, TextureTensor tensor, uint32_t textureIndex, size_t depth = 1);

		const uint8_t* GetTextureBuffer() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipsLevel() const;
		uint32_t GetChannels() const;
		TextureTensor GetTensor() const;
		uint32_t GetBindingSlot() const;
		uint32_t GetShaderRegister() const;
		uint32_t GetSpaceSet() const;
		uint32_t GetSamplerRegister() const;
		uint32_t GetTextureIndex() const;
	private:
		TextureTensor m_Tensor;
		std::shared_ptr<Image> m_Image;
		size_t m_Depth;
		uint32_t m_SpaceSet;
		uint32_t m_BindingSlot;
		uint32_t m_ShaderRegister;
		uint32_t m_SamplerRegister;
		uint32_t m_TextureIndex;
	};

	class SAMPLE_SHADER_MNG_DLL_COMMAND TextureLayout
	{
	public:
		TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages);

		const TextureElement& GetElement(uint32_t shaderRegister, uint32_t textureIndex);
		const std::unordered_map<uint32_t, TextureElement>& GetElements();

		uint32_t GetStages() const;

	private:
		std::unordered_map<uint32_t, TextureElement> m_Textures;
		uint32_t m_Stages;
	};
}