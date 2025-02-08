#pragma once

#include "ExecutionPipeline.hpp"
#include "VKContext.hpp"

namespace SampleRenderV2
{
	class VKExecutionPipeline : public ExecutionPipeline
	{
	public:
		VKExecutionPipeline(const std::shared_ptr<VKContext>* context);
		~VKExecutionPipeline();

		void Wait() override;
	
		VkQueue GetCopyQueue() const;
		VkCommandBuffer GetCommandBuffer() const;
		VkCommandPool GetCommandPool() const;

	private:
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice adapter);
		const std::shared_ptr<VKContext>* m_Context;

		VkCommandPool m_CopyCommandPool;
		VkCommandBuffer m_CopyCommandBuffer;
		VkQueue m_CopyQueue;
	};
}