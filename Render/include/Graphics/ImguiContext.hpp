#pragma once

#include "GraphicsContext.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND ImguiContext
	{
	public:
		virtual ~ImguiContext() = default;

		static void StartImgui();
		static void EndImgui();
		static void StartFrame();
		static void EndFrame();

		virtual void ReceiveInput() = 0;
		virtual void DispatchInput() = 0;

		static ImguiContext* Instantiate(const std::shared_ptr<GraphicsContext>* graphicsContext);
	};
}