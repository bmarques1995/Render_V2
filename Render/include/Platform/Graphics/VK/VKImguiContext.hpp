#pragma	once

#include "ImguiContext.hpp"
#include "VKContext.hpp"
#include <imgui/imgui_impl_vulkan.h>

namespace SampleRenderV2
{
	class VKImguiContext : public ImguiContext
	{
	public:
		VKImguiContext(const std::shared_ptr<VKContext>* vkContext);
		~VKImguiContext();

		void ReceiveInput() override;
		void DispatchInput() override;
	private:
		const std::shared_ptr<VKContext>* m_Context;
	};
}