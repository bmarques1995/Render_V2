#pragma once

#include "ImguiWindowController.hpp"
#include "SDLWindow.hpp"

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND SDLImguiWindowController : public ImguiWindowController
	{
	public:
		SDLImguiWindowController(const std::shared_ptr<SDLWindow>* window);
		~SDLImguiWindowController();

		void ReceiveInput() override;
	private:
		const std::shared_ptr<SDLWindow>* m_Window;
	};
}