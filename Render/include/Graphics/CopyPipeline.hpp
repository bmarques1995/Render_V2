#pragma once
#include "RenderDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND CopyPipeline
	{
	public:
		virtual ~CopyPipeline() = default;

		virtual void Wait() = 0;

		static CopyPipeline* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}