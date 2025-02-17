#pragma once

#ifdef RENDER_USES_WINDOWS

#include "ExecutionPipeline.hpp"
#include "D3D12Context.hpp"

namespace SampleRenderV2
{
	class D3D12ExecutionPipeline : public ExecutionPipeline
	{
	public:
		D3D12ExecutionPipeline(const std::shared_ptr<D3D12Context>* context);
		~D3D12ExecutionPipeline();

		void Wait() override;
	
		ID3D12GraphicsCommandList6* GetCommandList() const;
		ID3D12CommandAllocator* GetCommandAllocator() const;
		ID3D12CommandQueue* GetCommandQueue() const;
	
	private:
		ComPointer<ID3D12CommandAllocator> m_CopyCommandAllocator;
		ComPointer<ID3D12CommandQueue> m_CopyCommandQueue;
		ComPointer<ID3D12GraphicsCommandList6> m_CopyCommandList;
		ComPointer<ID3D12Fence> m_CopyFence;
		uint64_t m_CopyFenceValue = 0;
		HANDLE m_CopyFenceEvent;

		void WaitCopyPipeline(UINT64 fenceValue = -1);

		const std::shared_ptr<D3D12Context>* m_Context;
	};
}

#endif
