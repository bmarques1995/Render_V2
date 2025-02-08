#pragma once

#ifdef RENDER_USES_WINDOWS

#include "D3D12Context.hpp"
#include "D3D12ExecutionPipeline.hpp"
#include "GPUInstrumentator.hpp"
#include <memory>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND D3D12Instrumentator : public GPUInstrumentator
	{
	public:
		D3D12Instrumentator(const std::shared_ptr<D3D12Context>* context);
		~D3D12Instrumentator();

		void BeginQueryTime() override;
		void EndQueryTime() override;

		double GetQueryTime() override;
	private:
		const std::shared_ptr<D3D12Context>* m_Context;
		ComPointer<ID3D12QueryHeap> m_QueryHeap;
		uint32_t m_QueryCount;
		ComPointer<ID3D12Resource> m_QueryResult;
		ComPointer<ID3D12Resource> m_QueryVisibleBuffer;
		uint64_t m_Timestamp;
		uint64_t m_LastTime;
		uint64_t m_Frequency;
	};
}

#endif
