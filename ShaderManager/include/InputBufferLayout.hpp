#pragma once

#include "ShaderManagerDLLMacro.hpp"
#include <cstdint>
#include <cassert>
#include <string>
#include "Console.hpp"

namespace SampleRenderV2
{
	enum class SAMPLE_SHADER_MNG_DLL_COMMAND ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat4, Uint, Uint2, Uint3, Uint4, Bool
	};

	SAMPLE_SHADER_MNG_DLL_COMMAND uint32_t ShaderDataTypeSize(ShaderDataType type);
	

	class SAMPLE_SHADER_MNG_DLL_COMMAND InputBufferElement
	{
		friend class InputBufferLayout;
		friend class Shader;
	public:
		InputBufferElement();

		InputBufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

		uint32_t GetComponentCount() const;

		const std::string& GetName() const;
		const ShaderDataType GetType() const;
		const uint32_t GetSize() const;
		const uint32_t GetOffset() const;
		const bool IsNormalized() const;
	private:
		std::string m_Name;
		ShaderDataType m_Type;
		uint32_t m_Size;
		uint32_t m_Offset;
		bool m_Normalized;
	};

	class SAMPLE_SHADER_MNG_DLL_COMMAND InputBufferLayout
	{
	public:
		InputBufferLayout() {}

		InputBufferLayout(const std::initializer_list<InputBufferElement>& elements);
		InputBufferLayout(const std::vector<InputBufferElement>& elements);

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<InputBufferElement>& GetElements() const { return m_Elements; }

		std::vector<InputBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<InputBufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<InputBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<InputBufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride();
		
	private:
		std::vector<InputBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}