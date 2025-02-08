#pragma once

#include "RenderDLLMacro.hpp"
#include "GraphicsContext.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND GPUInstrumentator
	{
	public:
		virtual ~GPUInstrumentator() = default;

		virtual void BeginQueryTime() = 0;
		virtual void EndQueryTime() = 0;

		virtual double GetQueryTime() = 0;

		static GPUInstrumentator* Instantiate(const std::shared_ptr<GraphicsContext>* context);
	};
}