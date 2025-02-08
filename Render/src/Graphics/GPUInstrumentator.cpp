#include "GPUInstrumentator.hpp"
#include "Application.hpp"
#ifdef RENDER_USES_WINDOWS
#include "D3D12Instrumentator.hpp"
#endif
#include "VKInstrumentator.hpp"

SampleRenderV2::GPUInstrumentator* SampleRenderV2::GPUInstrumentator::Instantiate(const std::shared_ptr<GraphicsContext>* context)
{
	GraphicsAPI api = Application::GetInstance()->GetCurrentAPI();
	switch (api)
	{
#ifdef RENDER_USES_WINDOWS
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_D3D12:
	{
		return new D3D12Instrumentator((const std::shared_ptr<D3D12Context>*)(context));
	}
#endif
	case SampleRenderV2::SAMPLE_RENDER_GRAPHICS_API_VK:
	{
		return new VKInstrumentator((const std::shared_ptr<VKContext>*)(context));
	}
	default:
		break;
	}
	return nullptr;
}
