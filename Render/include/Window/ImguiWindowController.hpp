#pragma once

#include "Window.hpp"
#include <memory>

namespace SampleRenderV2
{
	class ImguiWindowController
	{
	public:
		virtual ~ImguiWindowController() = default;

		virtual void ReceiveInput() = 0;

		static ImguiWindowController* Instantiate(const std::shared_ptr<Window>* window);
	};
}