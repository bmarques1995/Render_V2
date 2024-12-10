#include "CompilerExceptions.hpp"

SampleRenderV2::CompilerException::CompilerException(std::string reason) :
	m_Exception(reason)
{
}

const char* SampleRenderV2::CompilerException::what() const
{
	return m_Exception.c_str();
}

SampleRenderV2::InvalidNameException::InvalidNameException(std::string reason) :
	CompilerException(reason)
{
}

SampleRenderV2::InvalidFilepathException::InvalidFilepathException(std::string reason) :
	CompilerException(reason)
{
}

SampleRenderV2::InvalidPipelineException::InvalidPipelineException(std::string reason) :
	CompilerException(reason)
{
}

SampleRenderV2::InvalidHLSLVersion::InvalidHLSLVersion(std::string reason) :
	CompilerException(reason)
{
}

SampleRenderV2::InvalidVulkanVersion::InvalidVulkanVersion(std::string reason) :
	CompilerException(reason)
{
}
