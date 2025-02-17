#ifdef RENDER_USES_WINDOWS

#include "D3D12ExecutionPipeline.hpp"
#include <cassert>

SampleRenderV2::D3D12ExecutionPipeline::D3D12ExecutionPipeline(const std::shared_ptr<D3D12Context>* context) :
	m_Context(context)
{
	auto device = (*m_Context)->GetDevicePtr();
	HRESULT hr;

	m_CopyFenceEvent = CreateEventW(nullptr, false, false, nullptr);

	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;
	hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CopyCommandQueue.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateFence(m_CopyFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_CopyFence.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CopyCommandAllocator.GetAddressOf()));
	assert(hr == S_OK);

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CopyCommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CopyCommandList.GetAddressOf()));
	assert(hr == S_OK);
}

SampleRenderV2::D3D12ExecutionPipeline::~D3D12ExecutionPipeline()
{
}

void SampleRenderV2::D3D12ExecutionPipeline::Wait()
{
	WaitCopyPipeline();
	m_CopyFenceEvent = CreateEventW(nullptr, false, false, nullptr);
}

ID3D12GraphicsCommandList6* SampleRenderV2::D3D12ExecutionPipeline::GetCommandList() const
{
	return m_CopyCommandList.GetConst();
}

ID3D12CommandAllocator* SampleRenderV2::D3D12ExecutionPipeline::GetCommandAllocator() const
{
	return m_CopyCommandAllocator.GetConst();
}

ID3D12CommandQueue* SampleRenderV2::D3D12ExecutionPipeline::GetCommandQueue() const
{
	return m_CopyCommandQueue.GetConst();
}

void SampleRenderV2::D3D12ExecutionPipeline::WaitCopyPipeline(UINT64 fenceValue)
{
	HRESULT hr;

	m_CopyCommandQueue->Signal(m_CopyFence.Get(), ++m_CopyFenceValue);

	bool continueWaiting = true;

	if (fenceValue == -1)
		fenceValue = m_CopyFenceValue;

	if ((m_CopyFence->GetCompletedValue() < fenceValue) && continueWaiting)
	{
		hr = m_CopyFence->SetEventOnCompletion(fenceValue, m_CopyFenceEvent);
		if (hr == S_OK)
			continueWaiting = (WaitForSingleObject(m_CopyFenceEvent, INFINITE) != WAIT_OBJECT_0);
		// Fallback wait
		while (m_CopyFence->GetCompletedValue() < fenceValue) Sleep(1);
	}

	CloseHandle(m_CopyFenceEvent);
}


#endif