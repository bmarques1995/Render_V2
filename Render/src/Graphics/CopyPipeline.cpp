#include "CopyPipeline.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12CopyPipeline.hpp"
#endif
#include "VKCopyPipeline.hpp"

SampleRenderV2::CopyPipeline* SampleRenderV2::CopyPipeline::Instantiate(const std::shared_ptr<GraphicsContext>* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12CopyPipeline((const std::shared_ptr<D3D12Context>*)(context));
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKCopyPipeline((const std::shared_ptr<VKContext>*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
