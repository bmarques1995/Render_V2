#include "TextureLayout.hpp"
#include <algorithm>

SampleRenderV2::TextureElement::TextureElement()
{
	uint32_t whitePixel = 0xffffffff;
	m_BindingSlot = 0xffff;
	m_Depth = 1;
	m_Tensor = TextureTensor::TENSOR_2;
	m_Image.reset(Image::CreateImage((const std::byte*)&whitePixel, 1, 1, ImageFormat::PNG));
	m_ShaderRegister = 0;
	m_SamplerRegister = 0;
	m_SpaceSet = 0;
	m_TextureIndex = 0;
}

//std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t samplerRegister, TextureTensor tensor, size_t depth = 1
SampleRenderV2::TextureElement::TextureElement(std::shared_ptr<Image> img, uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t samplerRegister, TextureTensor tensor, uint32_t textureIndex, size_t depth) :
	m_Image(img), m_Tensor(tensor), m_BindingSlot(bindingSlot), m_ShaderRegister(shaderRegister), m_SpaceSet(spaceSet), m_SamplerRegister(samplerRegister), m_TextureIndex(textureIndex)
{
	m_Depth = std::max<size_t>(1, depth);
}

const uint8_t* SampleRenderV2::TextureElement::GetTextureBuffer() const
{
	return m_Image->GetRawPointer();
}

uint32_t SampleRenderV2::TextureElement::GetWidth() const
{
	return m_Image->GetWidth();
}

uint32_t SampleRenderV2::TextureElement::GetHeight() const
{
	return m_Image->GetHeight();
}

uint32_t SampleRenderV2::TextureElement::GetDepth() const
{
	return m_Depth;
}

uint32_t SampleRenderV2::TextureElement::GetMipsLevel() const
{
	return m_Image->GetMips();
}

uint32_t SampleRenderV2::TextureElement::GetChannels() const
{
	return m_Image->GetChannels();
}

SampleRenderV2::TextureTensor SampleRenderV2::TextureElement::GetTensor() const
{
	return m_Tensor;
}

uint32_t SampleRenderV2::TextureElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t SampleRenderV2::TextureElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

uint32_t SampleRenderV2::TextureElement::GetSpaceSet() const
{
	return m_SpaceSet;
}

uint32_t SampleRenderV2::TextureElement::GetSamplerRegister() const
{
	return m_SamplerRegister;
}

uint32_t SampleRenderV2::TextureElement::GetTextureIndex() const
{
	return m_TextureIndex;
}

SampleRenderV2::TextureLayout::TextureLayout(std::initializer_list<TextureElement> elements, uint32_t allowedStages) :
	m_Stages(allowedStages)
{
		
	for (auto& element : elements)
	{
		uint64_t textureLocation = ((uint64_t)element.GetShaderRegister() << 32) + element.GetTextureIndex();
		m_Textures[textureLocation] = element;
	}
}

const SampleRenderV2::TextureElement& SampleRenderV2::TextureLayout::GetElement(uint32_t shaderRegister, uint32_t textureIndex)
{
	uint64_t textureLocation = ((uint64_t)shaderRegister << 32) + textureIndex;
	return m_Textures[textureLocation];
}

const std::unordered_map<uint64_t, SampleRenderV2::TextureElement>& SampleRenderV2::TextureLayout::GetElements()
{
	return m_Textures;
}

uint32_t SampleRenderV2::TextureLayout::GetStages() const
{
	return m_Stages;
}