#pragma once

#include <cstdint>
#include "CommonException.hpp"
#include <unordered_map>
#include "ShaderManagerDLLMacro.hpp"

namespace SampleRenderV2
{
	class SAMPLE_SHADER_MNG_DLL_COMMAND AttachmentMismatchException : public GraphicsException
	{
	public:
		AttachmentMismatchException(size_t bufferSize, size_t expectedBufferAttachment);
	};

	enum SAMPLE_SHADER_MNG_DLL_COMMAND AllowedStages
	{
		EMPTY_STAGE = 1 >> 1,
		VERTEX_STAGE = 1 << 0,
		PIXEL_STAGE = 1 << 1,
		GEOMETRY_STAGE = 1 << 2,
		HULL_STAGE = 1 << 3,
		DOMAIN_STAGE = 1 << 4,
		MESH_STAGE = 1 << 5,
		AMPLIFICATION_STAGE = 1 << 6
	};

	class SAMPLE_SHADER_MNG_DLL_COMMAND SmallBufferElement
	{
	public:
		SmallBufferElement();
		SmallBufferElement(size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment);

		size_t GetOffset() const;
		size_t GetSize() const;
		uint32_t GetBindingSlot() const;

		bool IsSizeValid(uint32_t smallAttachment);

	private:

		size_t m_Offset;
		size_t m_Size;
		uint32_t m_BindingSlot;
	};

	class SAMPLE_SHADER_MNG_DLL_COMMAND SmallBufferLayout
	{
	public:
		SmallBufferLayout(std::initializer_list<SmallBufferElement> m_Elements, uint32_t stages);

		const SmallBufferElement& GetElement(uint32_t bindingSlot);
		const std::unordered_map<uint32_t, SmallBufferElement>& GetElements();
		uint32_t GetStages() const;
	private:
		std::unordered_map<uint32_t, SmallBufferElement> m_Buffers;
		uint32_t m_Stages;
	};
}