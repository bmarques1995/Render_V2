#pragma once

#ifdef RENDER_USES_WINDOWS

#include "ImguiContext.hpp"
#include "D3D12Context.hpp"
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui.h>


namespace SampleRenderV2
{
    // Simple free list based allocator
    struct DescriptorHeapAllocator
    {
        ID3D12DescriptorHeap* Heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
        UINT                        HeapHandleIncrement;
        ImVector<int>               FreeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap);
        void Destroy();
        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle);
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);
    };
	class D3D12ImguiContext : public ImguiContext
	{
	public:
		D3D12ImguiContext(const std::shared_ptr<D3D12Context>* d3d12Context);
		~D3D12ImguiContext();

		void ReceiveInput() override;
        void DispatchInput() override;
	private:
        const std::shared_ptr<D3D12Context>* m_Context;
        ComPointer<ID3D12DescriptorHeap> m_ImGuiHeap;
        static DescriptorHeapAllocator* s_DescriptorHeapImguiAllocator;

        static void AllocateDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle);
        static void FreeDescriptor(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);
	};
}

#endif
