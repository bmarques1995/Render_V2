#include "ExecutionPipeline.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12ExecutionPipeline.hpp"
#endif
#include "VKExecutionPipeline.hpp"

SampleRenderV2::ExecutionPipeline* SampleRenderV2::ExecutionPipeline::Instantiate(const std::shared_ptr<GraphicsContext>* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12ExecutionPipeline((const std::shared_ptr<D3D12Context>*)(context));
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKExecutionPipeline((const std::shared_ptr<VKContext>*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
