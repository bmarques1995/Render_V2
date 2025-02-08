#pragma once

#include "VKContext.hpp"
#include "GPUInstrumentator.hpp"
#include "VKExecutionPipeline.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND VKInstrumentator : public GPUInstrumentator
	{
	public:
		VKInstrumentator(const std::shared_ptr<VKContext>* context);
		~VKInstrumentator();

		void BeginQueryTime() override;
		void EndQueryTime() override;

		double GetQueryTime() override;
	private:
		const std::shared_ptr<VKContext>* m_Context;
		VkQueryPool m_QueryPool;
		uint32_t m_QueryCount;
		uint64_t* m_Timestamp;
	};
}
