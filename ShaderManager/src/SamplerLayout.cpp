#include "SamplerLayout.hpp"

SampleRenderV2::SamplerElement::SamplerElement()
{
	m_AddressMode = AddressMode::REPEAT;
	m_AnisotropicFactor = AnisotropicFactor::FACTOR_0;
	m_ComparisonPassMode = ComparisonPassMode::NEVER;
	m_Filter = SamplerFilter::LINEAR;
	m_SpaceSet = 0;
	m_ShaderRegister = 0;
	m_BindingSlot = 0;
	m_SamplerIndex = 0;
}

SampleRenderV2::SamplerElement::SamplerElement(SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex) :
	m_Filter(filter),
	m_AnisotropicFactor(anisotropicFactor),
	m_AddressMode(addressMode),
	m_ComparisonPassMode(comparisonPassMode),
	m_BindingSlot(bindingSlot),
	m_SpaceSet(spaceSet),
	m_ShaderRegister(shaderRegister),
	m_SamplerIndex(samplerIndex)
{
}

SampleRenderV2::SamplerFilter SampleRenderV2::SamplerElement::GetFilter() const
{
	return m_Filter;
}

SampleRenderV2::AnisotropicFactor SampleRenderV2::SamplerElement::GetAnisotropicFactor() const
{
	return m_AnisotropicFactor;
}

SampleRenderV2::AddressMode SampleRenderV2::SamplerElement::GetAddressMode() const
{
	return m_AddressMode;
}

SampleRenderV2::ComparisonPassMode SampleRenderV2::SamplerElement::GetComparisonPassMode() const
{
	return m_ComparisonPassMode;
}

uint32_t SampleRenderV2::SamplerElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

uint32_t SampleRenderV2::SamplerElement::GetSpaceSet() const
{
	return m_SpaceSet;
}

uint32_t SampleRenderV2::SamplerElement::GetShaderRegister() const
{
	return m_ShaderRegister;
}

uint32_t SampleRenderV2::SamplerElement::GetSamplerIndex() const
{
	return m_SamplerIndex;
}

SampleRenderV2::SamplerLayout::SamplerLayout(std::initializer_list<SamplerElement> elements)
{
	for (auto& element : elements)
	{
		uint64_t samplerLocation = ((uint64_t)element.GetShaderRegister() << 32) + element.GetSamplerIndex();
		m_Samplers[samplerLocation] = element;
	}
}

const SampleRenderV2::SamplerElement& SampleRenderV2::SamplerLayout::GetElement(uint32_t shaderRegister, uint32_t samplerIndex)
{
	uint64_t samplerLocation = ((uint64_t)shaderRegister << 32) + samplerIndex;
	return m_Samplers[samplerLocation];
}

const std::unordered_map<uint64_t, SampleRenderV2::SamplerElement>& SampleRenderV2::SamplerLayout::GetElements()
{
	return m_Samplers;
}