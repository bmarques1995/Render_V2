#include "AppContext.hpp"
#include "ExampleLayer.hpp"

AppContext::AppContext(int argc, char** argv) :
	SampleRenderV2::Application(argc, argv)
{
	m_LayerStack.PushLayer(new ExampleLayer());
}

AppContext::AppContext(int argc, wchar_t** wargv) :
	SampleRenderV2::Application(argc, wargv)
{
	m_LayerStack.PushLayer(new ExampleLayer());
}
