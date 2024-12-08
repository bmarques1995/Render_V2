#include "CommonException.hpp"

char const* SampleRenderV2::GraphicsException::what() const
{
	return m_Reason.c_str();
}

SampleRenderV2::GraphicsException::GraphicsException() :
	m_Reason("")
{

}

SampleRenderV2::GraphicsException::GraphicsException(std::string reason) :
	m_Reason(reason)
{

}