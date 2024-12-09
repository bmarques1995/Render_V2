#include "VKImguiContext.hpp"

SampleRenderV2::VKImguiContext::VKImguiContext(const std::shared_ptr<VKContext>* vkContext) : 
	m_Context(vkContext)
{
	ImGui_ImplVulkan_InitInfo vulkanInfo = {};
	vulkanInfo.Instance = (*vkContext)->GetInstance();
	vulkanInfo.PhysicalDevice = (*vkContext)->GetAdapter();
	vulkanInfo.Device = (*vkContext)->GetDevice();
	vulkanInfo.Queue = (*vkContext)->GetGraphicsQueue();
	vulkanInfo.QueueFamily = (*vkContext)->GetGraphicsQueueFamilyIndex();
	vulkanInfo.DescriptorPoolSize = 2;
	vulkanInfo.RenderPass = (*vkContext)->GetRenderPass();
	vulkanInfo.MinImageCount = (*vkContext)->GetSwapchainImageCount();
	vulkanInfo.ImageCount = (*vkContext)->GetSwapchainImageCount();
	ImGui_ImplVulkan_Init(&vulkanInfo);
}

SampleRenderV2::VKImguiContext::~VKImguiContext()
{
	ImGui_ImplVulkan_Shutdown();
}

void SampleRenderV2::VKImguiContext::ReceiveInput()
{
	ImGui_ImplVulkan_NewFrame();
}

void SampleRenderV2::VKImguiContext::DispatchInput()
{
	auto cmdBuffer = (*m_Context)->GetCurrentCommandBuffer();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}
