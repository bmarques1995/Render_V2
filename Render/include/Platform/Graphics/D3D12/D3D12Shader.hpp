#pragma once

#include "Shader.hpp"
#include "D3D12Context.hpp"
#include "DXCSafeInclude.hpp"
#include <json/json.h>
#include <functional>

namespace SampleRenderV2
{
	struct ResourceAndHeap
	{
		ComPointer<ID3D12Resource2> Resource;
		ComPointer<ID3D12DescriptorHeap> Heap;
	};

	class SAMPLE_RENDER_DLL_COMMAND D3D12Shader : public Shader
	{
	public:
		D3D12Shader(const std::shared_ptr<D3D12Context>* context, std::string json_controller_path, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout);
		~D3D12Shader();

		void Stage() override;
		virtual uint32_t GetStride() const override;
		virtual uint32_t GetOffset() const override;

		void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot) override;

		void BindDescriptors() override;

		void UpdateCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex) override;
	private:

		bool IsCBufferValid(size_t size);
		void PreallocateRootCBuffer(const void* data, UniformElement uniformElement);
		void PreallocateTabledCBuffer(const void* data, UniformElement uniformElement);
		void MapCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex = 1);

		void CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device10* device);
		void BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);

		void PushShader(std::string_view stage, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc);
		void InitJsonAndPaths(std::string json_controller_path);

		static DXGI_FORMAT GetNativeFormat(ShaderDataType type);
		static D3D12_DESCRIPTOR_HEAP_TYPE GetNativeHeapType(BufferType type);
		static D3D12_RESOURCE_DIMENSION GetNativeDimension(BufferType type);
		static const std::unordered_map<std::string, std::function<void(IDxcBlob**, D3D12_GRAPHICS_PIPELINE_STATE_DESC*)>> s_ShaderPusher;
		static const std::list<std::string> s_GraphicsPipelineStages;

		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_RootDescriptors;
		std::unordered_map<uint32_t, ComPointer<ID3D12DescriptorHeap>> m_TabledDescriptors;
		//descriptor index, resource index
		std::unordered_map<uint64_t, ComPointer<ID3D12Resource2>> m_CBVResources;


		Json::Value m_PipelineInfo;

		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		const std::shared_ptr<D3D12Context>* m_Context;
		std::string m_ShaderDir;
		ComPointer<IDxcBlob> m_RootBlob;
		ComPointer<ID3D12PipelineState> m_GraphicsPipeline;
		std::unordered_map<std::string, ComPointer<IDxcBlob>> m_ShaderBlobs;
		ComPointer<ID3D12RootSignature> m_RootSignature;
		/*in bytes*/
		uint32_t m_RootSignatureSize = 0;
	};
}