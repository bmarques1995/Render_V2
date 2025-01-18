#include "UniformsLayout.hpp"
#include <sstream>

SampleRenderV2::AttachmentMismatchException::AttachmentMismatchException(size_t bufferSize, size_t expectedBufferAttachment)
{
	std::stringstream buffer;
	buffer << "Is expected the buffer be multiple of " << expectedBufferAttachment <<
		", but the module of the division between the buffer size and the expected attachment is " << (bufferSize % expectedBufferAttachment);
	m_Reason = buffer.str();
}

SampleRenderV2::SmallBufferElement::SmallBufferElement()
{
	m_Offset = 0;
	m_Size = 0;
	m_BindingSlot = 0xffff;
}

SampleRenderV2::SmallBufferElement::SmallBufferElement(size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment) :
	m_Offset(offset), m_Size(size), m_BindingSlot(bindingSlot)
{
	if (!IsSizeValid(smallAttachment))
		throw AttachmentMismatchException(size, smallAttachment);
}

size_t SampleRenderV2::SmallBufferElement::GetOffset() const
{
	return m_Offset;
}

size_t SampleRenderV2::SmallBufferElement::GetSize() const
{
	return m_Size;
}

uint32_t SampleRenderV2::SmallBufferElement::GetBindingSlot() const
{
	return m_BindingSlot;
}

bool SampleRenderV2::SmallBufferElement::IsSizeValid(uint32_t smallAttachment)
{
	return ((m_Size % smallAttachment) == 0);
}

SampleRenderV2::SmallBufferLayout::SmallBufferLayout(std::initializer_list<SmallBufferElement> m_Elements, uint32_t stages) :
	m_Stages(stages)
{
	for (auto& element : m_Elements)
	{
		m_Buffers[element.GetBindingSlot()] = element;
	}
}

const SampleRenderV2::SmallBufferElement& SampleRenderV2::SmallBufferLayout::GetElement(uint32_t bindingSlot)
{
	return m_Buffers[bindingSlot];
}

const std::unordered_map<uint32_t, SampleRenderV2::SmallBufferElement>& SampleRenderV2::SmallBufferLayout::GetElements()
{
	return m_Buffers;
}

uint32_t SampleRenderV2::SmallBufferLayout::GetStages() const
{
	return m_Stages;
}
