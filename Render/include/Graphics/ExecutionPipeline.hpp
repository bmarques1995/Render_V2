#pragma once
#include "RenderDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND ExecutionPipeline
	{
	public:
		virtual ~ExecutionPipeline() = default;

		virtual void Wait() = 0;

		static ExecutionPipeline* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}