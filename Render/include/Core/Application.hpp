#pragma once

#include "RenderDLLMacro.hpp"
#include "Window.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND Application
	{
	public:
		Application();
		~Application();

		void Run();
	private:
		std::shared_ptr<Window> m_Window;
	};
}
