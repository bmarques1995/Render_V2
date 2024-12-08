#pragma once

#include "UtilsDLLMacro.hpp"
#include <exception>
#include <string>

namespace SampleRenderV2
{
	class SAMPLE_UTILS_DLL_COMMAND GraphicsException : public std::exception
	{
	public:
		GraphicsException(std::string reason);

		char const* what() const override;
	protected:
		GraphicsException();

		std::string m_Reason;
	};
}