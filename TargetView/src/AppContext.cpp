#include "AppContext.hpp"
#include "ExampleLayer.hpp"

AppContext::AppContext()
{
	m_LayerStack.PushLayer(new ExampleLayer());
}
