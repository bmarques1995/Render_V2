#include "D3D12Texture.hpp"
#include <cassert>
#include "D3D12CopyPipeline.hpp"
#include "Application.hpp"
#include "DDSTextureLoader12.h"
#include <vector>
#include "d3dx12.h"

SampleRenderV2::D3D12Texture2D::D3D12Texture2D(const std::shared_ptr<D3D12Context>* context, const TextureElement& specification) :
	m_Context(context), m_Specification(specification)
{
	m_Loaded = false;
	CreateResource();
	CopyBuffer();
	m_Loaded = true;
	m_Specification.FreeImage();
}

SampleRenderV2::D3D12Texture2D::D3D12Texture2D(const std::shared_ptr<D3D12Context>* context, const TextureElement& specification, std::string native_texture_path) :
	m_Context(context), m_Specification(specification)
{
	auto device = (*m_Context)->GetDevicePtr();
	HRESULT hr;
	m_Loaded = false;

	std::wstring native_texture_path_w(native_texture_path.begin(), native_texture_path.end());
	
	ComPointer<ID3D12Resource2> texture;
	std::unique_ptr<uint8_t[]> pixels;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::DDS_ALPHA_MODE alphaMode;
	bool isCubeMap;

	hr = DirectX::LoadDDSTextureFromFile(device, native_texture_path_w.c_str(), (ID3D12Resource**)texture.GetAddressOf(), pixels, subresources, 0, &alphaMode, &isCubeMap);
	assert(hr == S_OK);

	UpdateTextureInfo(texture->GetDesc1());
	CreateResource(texture->GetDesc1());
	CopyDDSBuffer(m_Texture.Get(), &subresources);

	m_Loaded = true;
	m_Specification.FreeImage();
}

SampleRenderV2::D3D12Texture2D::~D3D12Texture2D()
{
}

const SampleRenderV2::TextureElement& SampleRenderV2::D3D12Texture2D::GetTextureDescription() const
{
	return m_Specification;
}

uint32_t SampleRenderV2::D3D12Texture2D::GetWidth() const
{
	return m_Specification.GetWidth();
}

uint32_t SampleRenderV2::D3D12Texture2D::GetHeight() const
{
	return m_Specification.GetHeight();
}

bool SampleRenderV2::D3D12Texture2D::IsLoaded() const
{
	return m_Loaded;
}

ID3D12Resource2* SampleRenderV2::D3D12Texture2D::GetResource() const
{
	return m_Texture.GetConst();
}

void SampleRenderV2::D3D12Texture2D::UpdateTextureInfo(const D3D12_RESOURCE_DESC1& desc)
{
	m_Specification.m_Width = desc.Width;
	m_Specification.m_Height = desc.Height;
	m_Specification.m_Depth = desc.DepthOrArraySize;
	m_Specification.m_MipsLevel = desc.MipLevels;
	m_Specification.m_Channels = 4;
}

void SampleRenderV2::D3D12Texture2D::CreateResource()
{
	auto device = (*m_Context)->GetDevicePtr();
	HRESULT hr;

	D3D12_RESOURCE_DESC1 textureBufferDesc = {};
	textureBufferDesc.Dimension = GetNativeTensor(m_Specification.GetTensor());
	textureBufferDesc.Alignment = 0;
	textureBufferDesc.Width = m_Specification.GetWidth(); //mandatory
	textureBufferDesc.Height = m_Specification.GetHeight(); // mandatory 2 and 3
	textureBufferDesc.DepthOrArraySize = m_Specification.GetDepth(); // mandatory 3
	textureBufferDesc.MipLevels = m_Specification.GetMipsLevel(); // param
	textureBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureBufferDesc.SampleDesc.Count = 1;
	textureBufferDesc.SampleDesc.Quality = 0;
	textureBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&textureBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_Texture.GetAddressOf()));

	assert(hr == S_OK);
}

void SampleRenderV2::D3D12Texture2D::CreateResource(const D3D12_RESOURCE_DESC1& desc)
{
	auto device = (*m_Context)->GetDevicePtr();
	HRESULT hr;

	D3D12_RESOURCE_DESC1 textureBufferDesc = {};
	textureBufferDesc.Dimension = desc.Dimension;
	textureBufferDesc.Alignment = 0;
	textureBufferDesc.Width = desc.Width; //mandatory
	textureBufferDesc.Height = desc.Height; // mandatory 2 and 3
	textureBufferDesc.DepthOrArraySize = desc.DepthOrArraySize; // mandatory 3
	textureBufferDesc.MipLevels = desc.MipLevels; // param
	textureBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureBufferDesc.SampleDesc.Count = 1;
	textureBufferDesc.SampleDesc.Quality = 0;
	textureBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	hr = device->CreateCommittedResource2(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&textureBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		nullptr,
		IID_PPV_ARGS(m_Texture.GetAddressOf()));

	assert(hr == S_OK);
}

void SampleRenderV2::D3D12Texture2D::CopyBuffer()
{
	HRESULT hr;
	auto device = (*m_Context)->GetDevicePtr();
	ComPointer<ID3D12Resource2> textureBuffer;
	std::shared_ptr<D3D12CopyPipeline>* copyPipeline = (std::shared_ptr<D3D12CopyPipeline>*)
		(Application::GetInstance()->GetCopyPipeline());

	auto copyCommandList = (*copyPipeline)->GetCommandList();
	auto copyCommandAllocator = (*copyPipeline)->GetCommandAllocator();
	auto copyCommandQueue = (*copyPipeline)->GetCommandQueue();

	D3D12_RESOURCE_DESC uploadBufferDesc = {};
	uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadBufferDesc.Width = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());
	uploadBufferDesc.Height = 1;
	uploadBufferDesc.DepthOrArraySize = 1;
	uploadBufferDesc.MipLevels = 1;
	uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadBufferDesc.SampleDesc.Count = 1;
	uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	hr = device->CreateCommittedResource1(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  // Upload heap is CPU-accessible
		nullptr,
		nullptr,
		IID_PPV_ARGS(textureBuffer.GetAddressOf())
	);

	assert(hr == S_OK);

	D3D12_RANGE readRange = { 0 };
	void* gpuData = nullptr;
	hr = textureBuffer->Map(0, &readRange, &gpuData);
	assert(hr == S_OK);
	size_t textureSize = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());
	memcpy(gpuData, m_Specification.GetTextureBuffer(), textureSize);
	textureBuffer->Unmap(0, NULL);

	D3D12_TEXTURE_COPY_LOCATION destLocation = {};
	destLocation.pResource = m_Texture;
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destLocation.SubresourceIndex = 0;

	D3D12_RESOURCE_DESC1 textureDesc = m_Texture->GetDesc1();

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = textureBuffer.Get();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	//3rd arg represents the number of subresources
	device->GetCopyableFootprints1(&textureDesc, 0, 1, 0, &srcLocation.PlacedFootprint, nullptr, nullptr, nullptr);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_Texture;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyCommandList->ResourceBarrier(1, &barrier);

	copyCommandList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

	// Transition the resource to PIXEL_SHADER_RESOURCE for sampling

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_Texture;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	copyCommandList->ResourceBarrier(1, &barrier);

	copyCommandList->Close();

	ID3D12CommandList* lists[] = { copyCommandList };
	copyCommandQueue->ExecuteCommandLists(1, lists);

	(*copyPipeline)->Wait();

	copyCommandAllocator->Reset();
	copyCommandList->Reset(copyCommandAllocator, nullptr);
}

void SampleRenderV2::D3D12Texture2D::CopyDDSBuffer(ID3D12Resource2* buffer, std::vector<D3D12_SUBRESOURCE_DATA>* subresources)
{
	auto device = (*m_Context)->GetDevicePtr();
	HRESULT hr;
	std::shared_ptr<D3D12CopyPipeline>* copyPipeline = (std::shared_ptr<D3D12CopyPipeline>*)
		(Application::GetInstance()->GetCopyPipeline());

	auto copyCommandList = (*copyPipeline)->GetCommandList();
	auto copyCommandAllocator = (*copyPipeline)->GetCommandAllocator();
	auto copyCommandQueue = (*copyPipeline)->GetCommandQueue();

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer, 0,
		static_cast<UINT>(subresources->size()));

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	auto desc2 = (D3D12_RESOURCE_DESC)(desc);

	ComPointer<ID3D12Resource> uploadRes;
	hr = device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadRes.GetAddressOf()));

	UpdateSubresources(copyCommandList, buffer, uploadRes.Get(),
		0, 0, static_cast<UINT>(subresources->size()), subresources->data());

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	copyCommandList->ResourceBarrier(1, &barrier);

	hr = copyCommandList->Close();
	assert(hr == S_OK);

	ID3D12CommandList* lists[] = { copyCommandList };
	copyCommandQueue->ExecuteCommandLists(1, lists);

	(*copyPipeline)->Wait();

	copyCommandAllocator->Reset();
	copyCommandList->Reset(copyCommandAllocator, nullptr);
}

D3D12_RESOURCE_DIMENSION SampleRenderV2::D3D12Texture2D::GetNativeTensor(TextureTensor tensor)
{
	switch (tensor)
	{
	case TextureTensor::TENSOR_1:
		return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
	case TextureTensor::TENSOR_2:
		return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	case TextureTensor::TENSOR_3:
		return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	default:
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}
