#include "VKImguiContext.hpp"
#include "Application.hpp"

#ifdef RENDER_USES_WINDOWS
// FIXME: This code would ideally be inside imgui_impl_win32.cpp, it would create a dependency on Vulkan headers in imgui_impl_win32.cpp
int SampleRenderV2::VKImguiContext::ImGui_ImplWin32_CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface)
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = (HWND)viewport->PlatformHandleRaw;
	createInfo.hinstance = ::GetModuleHandle(nullptr);
	return (int)vkCreateWin32SurfaceKHR((VkInstance)vk_instance, &createInfo, (VkAllocationCallbacks*)vk_allocator, (VkSurfaceKHR*)out_vk_surface);
}
#endif

SampleRenderV2::VKImguiContext::VKImguiContext(const std::shared_ptr<VKContext>* vkContext) : 
	m_Context(vkContext)
{
#ifdef RENDER_USES_WINDOWS
	ImGui::GetPlatformIO().Platform_CreateVkSurface = ImGui_ImplWin32_CreateVkSurface;
#endif
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
