#include "VKInstrumentator.hpp"
#include <cassert>

SampleRenderV2::VKInstrumentator::VKInstrumentator(const std::shared_ptr<VKContext>* context) :
	m_QueryCount(2), m_Context(context)
{
	VkResult vkr;
	auto device = (*m_Context)->GetDevice();

	VkQueryPoolCreateInfo queryPoolCreateInfo = {};
	queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	queryPoolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
	queryPoolCreateInfo.queryCount = m_QueryCount;  // Number of timestamps
	
	vkr = vkCreateQueryPool(device, &queryPoolCreateInfo, nullptr, &m_QueryPool);
	assert(vkr == VK_SUCCESS);

	m_Timestamp = new uint64_t[m_QueryCount];
}

SampleRenderV2::VKInstrumentator::~VKInstrumentator()
{
	delete[] m_Timestamp;
	auto device = (*m_Context)->GetDevice();
	vkDeviceWaitIdle(device);
	vkDestroyQueryPool((*m_Context)->GetDevice(), m_QueryPool, nullptr);
}

void SampleRenderV2::VKInstrumentator::BeginQueryTime()
{
	auto cmdBuffer = (*m_Context)->GetCurrentCommandBuffer();
	vkCmdResetQueryPool(cmdBuffer, m_QueryPool, 0, m_QueryCount);  // Reset the queries
	vkCmdWriteTimestamp2(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_QueryPool, 0);
}

void SampleRenderV2::VKInstrumentator::EndQueryTime()
{
	auto cmdBuffer = (*m_Context)->GetCurrentCommandBuffer();
	vkCmdWriteTimestamp2(cmdBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_QueryPool, 1);
}

double SampleRenderV2::VKInstrumentator::GetQueryTime()
{
	auto device = (*m_Context)->GetDevice();
	VkResult vkr;
	vkr = vkGetQueryPoolResults(device, m_QueryPool, 0, 2, sizeof(m_Timestamp) * m_QueryCount, m_Timestamp, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
	if (vkr == VK_SUCCESS) {
		// Calculate elapsed time between the timestamps
		uint64_t elapsedTime = m_Timestamp[1] - m_Timestamp[0];
		double result = elapsedTime * 1.0e-9;  // Convert to seconds
		return result;
	}
	return 0.0f;
}
