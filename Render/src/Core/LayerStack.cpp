#include "LayerStack.hpp"

SampleRenderV2::LayerStack::LayerStack()
{
	m_LayerInsert = 0;
}

SampleRenderV2::LayerStack::~LayerStack()
{
	for (Layer* layer : m_Layers)
	{
		delete layer;
	}
}

void SampleRenderV2::LayerStack::PushLayer(Layer* layer)
{
	m_Layers.insert(advance(m_LayerInsert), layer);
	layer->OnAttach();
	m_LayerInsert++;
}

void SampleRenderV2::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.push_back(overlay);
	overlay->OnAttach();
}

void SampleRenderV2::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
	{
		(*it)->OnDetach();
		m_Layers.erase(it);
		m_LayerInsert--;
	}
}

void SampleRenderV2::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		(*it)->OnDetach();
		m_Layers.erase(it);
	}
}
