#include "InputBufferLayout.hpp"

uint32_t SampleRenderV2::ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return 4;
	case ShaderDataType::Float2:   return 4 * 2;
	case ShaderDataType::Float3:   return 4 * 3;
	case ShaderDataType::Float4:   return 4 * 4;
	case ShaderDataType::Mat4:     return 4 * 4 * 4;
	case ShaderDataType::Uint:      return 4;
	case ShaderDataType::Uint2:     return 4 * 2;
	case ShaderDataType::Uint3:     return 4 * 3;
	case ShaderDataType::Uint4:     return 4 * 4;
	case ShaderDataType::Bool:     return 1;
	}

	Console::CoreError("Unknown ShaderDataType!");
	assert(false);
	return 0;
}

SampleRenderV2::InputBufferElement::InputBufferElement()
{
	m_Name = "";
	m_Type = ShaderDataType::None;
	m_Size = 0;
	m_Offset = 0;
	m_Normalized = false;
}

SampleRenderV2::InputBufferElement::InputBufferElement(ShaderDataType type, const std::string& name, bool normalized) :
	m_Name(name), m_Type(type), m_Size(ShaderDataTypeSize(type)), m_Offset(0), m_Normalized(normalized)
{
}

uint32_t SampleRenderV2::InputBufferElement::GetComponentCount() const
{
	switch (m_Type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;
	case ShaderDataType::Mat4:    return 4 * 4;
	case ShaderDataType::Uint:     return 1;
	case ShaderDataType::Uint2:    return 2;
	case ShaderDataType::Uint3:    return 3;
	case ShaderDataType::Uint4:    return 4;
	case ShaderDataType::Bool:    return 1;
	}

	Console::CoreError("Unknown ShaderDataType!");
	assert(false);
	return 0;
}

const std::string& SampleRenderV2::InputBufferElement::GetName() const
{
	return m_Name;
}

const SampleRenderV2::ShaderDataType SampleRenderV2::InputBufferElement::GetType() const
{
	return m_Type;
}

const uint32_t SampleRenderV2::InputBufferElement::GetSize() const
{
	return m_Size;
}

const uint32_t SampleRenderV2::InputBufferElement::GetOffset() const
{
	return m_Offset;
}

const bool SampleRenderV2::InputBufferElement::IsNormalized() const
{
	return m_Normalized;
}

SampleRenderV2::InputBufferLayout::InputBufferLayout(const std::initializer_list<InputBufferElement>& elements) :
	m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

SampleRenderV2::InputBufferLayout::InputBufferLayout(const std::vector<InputBufferElement>& elements) :
	m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

void SampleRenderV2::InputBufferLayout::CalculateOffsetsAndStride()
{
	uint32_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.m_Offset = offset;
		offset += element.m_Size;
		m_Stride += element.m_Size;
	}
}
