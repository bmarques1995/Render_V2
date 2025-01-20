#pragma once

#include "Shader.hpp"
#include "VKContext.hpp"
#include "DXCSafeInclude.hpp"
#include <json/json.h>
#include <functional>

namespace SampleRenderV2
{
	class SAMPLE_RENDER_DLL_COMMAND VKShader : public Shader
	{
	public:
		VKShader(const std::shared_ptr<VKContext>* context, std::string json_controller_path, InputBufferLayout layout, SmallBufferLayout smallBufferLayout, UniformLayout uniformLayout);
		~VKShader();

		void Stage() override;
		virtual uint32_t GetStride() const override;
		virtual uint32_t GetOffset() const override;

		void BindSmallBuffer(const void* data, size_t size, uint32_t bindingSlot) override;

		void BindDescriptors() override;

		void UpdateCBuffer(const void* data, size_t size, uint32_t shaderRegister, uint32_t tableIndex) override;
	private:

		void CreateDescriptorSetLayout();

		void PushShader(std::string_view stage, VkPipelineShaderStageCreateInfo* graphicsDesc);
		void InitJsonAndPaths(std::string json_controller_path);
		void SetRasterizer(VkPipelineRasterizationStateCreateInfo* rasterizer);
		void SetInputAssemblyViewportAndMultisampling(VkPipelineInputAssemblyStateCreateInfo* inputAssembly, VkPipelineViewportStateCreateInfo* viewportState, VkPipelineMultisampleStateCreateInfo* multisampling);
		void SetBlend(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending);
		void SetDepthStencil(VkPipelineDepthStencilStateCreateInfo* depthStencil);

		static VkFormat GetNativeFormat(ShaderDataType type);
		static const std::list<std::string> s_GraphicsPipelineStages;

		static const std::unordered_map<std::string, VkShaderStageFlagBits> s_StageCaster;
		static const std::unordered_map<uint32_t, VkShaderStageFlagBits> s_EnumStageCaster;

		std::unordered_map<std::string, VkShaderModule> m_Modules;
		std::unordered_map<std::string, std::string> m_ModulesEntrypoint;

		Json::Value m_PipelineInfo;

		VkDescriptorSetLayout m_RootSignature;
		InputBufferLayout m_Layout;
		SmallBufferLayout m_SmallBufferLayout;
		UniformLayout m_UniformLayout;
		const std::shared_ptr<VKContext>* m_Context;
		std::string m_ShaderDir;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}