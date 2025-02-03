#include "VKTexture.hpp"
#include <cassert>
#include "VKCopyPipeline.hpp"
#include "Application.hpp"

#include <vulkan/vulkan.hpp>

typedef struct user_cbdata_optimal {
    VkBufferImageCopy* region; // Specify destination region in final image.
    VkDeviceSize offset;       // Offset of current level in staging buffer
    ktx_uint32_t numFaces;
    ktx_uint32_t numLayers;
    // The following are used only by optimalTilingPadCallback
    ktx_uint8_t* dest;         // Pointer to mapped staging buffer.
    ktx_uint32_t elementSize;
    ktx_uint32_t numDimensions;
#if defined(_DEBUG)
    VkBufferImageCopy* regionsArrayEnd;
#endif
} user_cbdata_optimal;

SampleRenderV2::VKTexture2D::VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification) :
    m_Context(context), m_Specification(specification), m_UsingKTX(false)
{
    m_Loaded = false;
    CreateResource();
    CopyBuffer();
    m_Loaded = true;
    m_Specification.FreeImage();
}

SampleRenderV2::VKTexture2D::VKTexture2D(const std::shared_ptr<VKContext>* context, const TextureElement& specification, std::string native_texture_path) :
    m_Context(context), m_Specification(specification), m_UsingKTX(false)
{
    m_Loaded = false;
    ktxTexture2* kTextureBuffer;
    CreateKTXBuffer(native_texture_path, &kTextureBuffer);
    CreateKTXResourceRaw(kTextureBuffer);
    CopyKTXBufferRaw(kTextureBuffer);
    UpdateTextureInfo(*kTextureBuffer);
    ktxTexture2_Destroy(kTextureBuffer);
    m_Loaded = true;
    m_Specification.FreeImage();
}

SampleRenderV2::VKTexture2D::~VKTexture2D()
{
    auto device = (*m_Context)->GetDevice();
    vkDeviceWaitIdle(device);
    vkDestroyImageView(device, m_ResourceView, nullptr);
    vkFreeMemory(device, m_Memory, nullptr);
    vkDestroyImage(device, m_Resource, nullptr);
}

const SampleRenderV2::TextureElement& SampleRenderV2::VKTexture2D::GetTextureDescription() const
{
	return m_Specification;
}

uint32_t SampleRenderV2::VKTexture2D::GetWidth() const
{
	return m_Specification.GetWidth();
}

uint32_t SampleRenderV2::VKTexture2D::GetHeight() const
{
	return m_Specification.GetHeight();
}

bool SampleRenderV2::VKTexture2D::IsLoaded() const
{
	return m_Loaded;
}

VkImage SampleRenderV2::VKTexture2D::GetResource() const
{
	return m_Resource;
}

VkDeviceMemory SampleRenderV2::VKTexture2D::GetMemory() const
{
	return m_Memory;
}

VkImageView SampleRenderV2::VKTexture2D::GetView() const
{
	return m_ResourceView;
}

void SampleRenderV2::VKTexture2D::CreateResource()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = GetNativeTensor(m_Specification.GetTensor());
    //params
    imageInfo.extent.width = m_Specification.GetWidth();
    imageInfo.extent.height = m_Specification.GetHeight();
    imageInfo.extent.depth = m_Specification.GetDepth();
    imageInfo.mipLevels = m_Specification.GetMipsLevel();

    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateImage(device, &imageInfo, nullptr, &m_Resource);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, m_Resource, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory);
    assert(vkr == VK_SUCCESS);

    vkr = vkBindImageMemory(device, m_Resource, m_Memory, 0);
    assert(vkr == VK_SUCCESS);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Resource;
    viewInfo.viewType = GetNativeTensorView(m_Specification.GetTensor());
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vkr = vkCreateImageView(device, &viewInfo, nullptr, &m_ResourceView);
    assert(vkr == VK_SUCCESS);
}

void SampleRenderV2::VKTexture2D::CopyBuffer()
{
    VkResult vkr;
    auto device = (*m_Context)->GetDevice();
    std::shared_ptr<VKCopyPipeline>* copyPipeline = (std::shared_ptr<VKCopyPipeline>*)
        (Application::GetInstance()->GetCopyPipeline());

    auto copyCommandBuffer = (*copyPipeline)->GetCommandBuffer();
    auto copyCommandPool = (*copyPipeline)->GetCommandPool();

    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    size_t imageSize = (m_Specification.GetWidth() * m_Specification.GetHeight() * m_Specification.GetDepth() * m_Specification.GetChannels());

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkr = vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    assert(vkr == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
    assert(vkr == VK_SUCCESS);

    vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

    void* GPUData = nullptr;
    vkr = vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &GPUData);
    assert(vkr == VK_SUCCESS);
    memcpy(GPUData, m_Specification.GetTextureBuffer(), imageSize);
    vkUnmapMemory(device, stagingBufferMemory);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        m_Specification.GetWidth(),
        m_Specification.GetHeight(),
        m_Specification.GetDepth()
    };

    vkCmdCopyBufferToImage(copyCommandBuffer, stagingBuffer, m_Resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkEndCommandBuffer(copyCommandBuffer);

    (*copyPipeline)->Wait();

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void SampleRenderV2::VKTexture2D::UpdateTextureInfo(const ktxTexture2& kTexture)
{
    m_Specification.m_Width = kTexture.baseWidth;
    m_Specification.m_Height = kTexture.baseHeight;
    m_Specification.m_Depth = kTexture.baseDepth;
    m_Specification.m_MipsLevel = kTexture.numLevels;
    m_Specification.m_Channels = 4;
}

void SampleRenderV2::VKTexture2D::CreateKTXBuffer(std::string native_texture_path, ktxTexture2** kTextureBuffer)
{
    ktxResult ktxr;
    auto adapter = (*m_Context)->GetAdapter();
    ktxr = ktxTexture2_CreateFromNamedFile(native_texture_path.c_str(),
        KTX_TEXTURE_CREATE_NO_FLAGS,
        kTextureBuffer);

    if (ktxTexture2_NeedsTranscoding(*kTextureBuffer))
    {
        ktx_transcode_fmt_e tf = KTX_TTF_ASTC_4x4_RGBA;

        // Using VkGetPhysicalDeviceFeatures or GL_COMPRESSED_TEXTURE_FORMATS or
        // extension queries, determine what compressed texture formats are
        // supported and pick a format. For example
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(adapter, &deviceFeatures);
        khr_df_model_e colorModel = ktxTexture2_GetColorModel_e(*kTextureBuffer);
        if (colorModel == KHR_DF_MODEL_UASTC
            && deviceFeatures.textureCompressionASTC_LDR) {
            tf = KTX_TTF_ASTC_4x4_RGBA;
        }
        else if (colorModel == KHR_DF_MODEL_ETC1S
            && deviceFeatures.textureCompressionETC2) {
            tf = KTX_TTF_ETC;
        }
        else if (deviceFeatures.textureCompressionASTC_LDR) {
            tf = KTX_TTF_ASTC_4x4_RGBA;
        }
        else if (deviceFeatures.textureCompressionETC2)
            tf = KTX_TTF_ETC2_RGBA;
        else if (deviceFeatures.textureCompressionBC)
            tf = KTX_TTF_BC3_RGBA;
        else {
            Console::CoreError("Vulkan implementation does not support any available transcode target.");
        }

        ktxr = ktxTexture2_TranscodeBasis(*kTextureBuffer, tf, 0);
    }
}

void SampleRenderV2::VKTexture2D::CreateKTXResourceRaw(ktxTexture2* rawTexture)
{
    VkResult vkr;
    ktxResult ktxr;
    auto device = (*m_Context)->GetDevice();

    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo allocInfo{};
    VkMemoryPropertyFlags properties;

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = rawTexture->numLevels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = rawTexture->numLayers;

    VkImageCreateInfo        imageCreateInfo = {
         .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
         .pNext = NULL
    };
    // Create optimal tiled target image
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.flags = 0;
    imageCreateInfo.format = (VkFormat)rawTexture->vkFormat;
    // numImageLevels ensures enough levels for generateMipmaps.
    imageCreateInfo.mipLevels = rawTexture->numLevels;
    imageCreateInfo.arrayLayers = rawTexture->numLayers;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent.width = rawTexture->baseWidth;
    imageCreateInfo.extent.height = rawTexture->baseHeight;
    imageCreateInfo.extent.depth = rawTexture->baseDepth;

    vkr = vkCreateImage(device, &imageCreateInfo, nullptr, &m_Resource);
    assert(vkr == VK_SUCCESS);

    memRequirements = {};
    vkGetImageMemoryRequirements(device, m_Resource, &memRequirements);

    properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory);
    assert(vkr == VK_SUCCESS);

    vkr = vkBindImageMemory(device, m_Resource, m_Memory, 0);
    assert(vkr == VK_SUCCESS);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Resource;
    viewInfo.viewType = GetNativeTensorView(m_Specification.GetTensor());
    viewInfo.format = (VkFormat)rawTexture->vkFormat;
    viewInfo.subresourceRange = subresourceRange;
    vkr = vkCreateImageView(device, &viewInfo, nullptr, &m_ResourceView);
    assert(vkr == VK_SUCCESS);
}

void SampleRenderV2::VKTexture2D::CopyKTXBufferRaw(ktxTexture2* rawTexture)
{
    VkResult vkr;
    ktxResult ktxr;
    auto device = (*m_Context)->GetDevice();
    std::shared_ptr<VKCopyPipeline>* copyPipeline = (std::shared_ptr<VKCopyPipeline>*)
        (Application::GetInstance()->GetCopyPipeline());

    auto copyCommandBuffer = (*copyPipeline)->GetCommandBuffer();
    auto copyCommandPool = (*copyPipeline)->GetCommandPool();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo allocInfo{};
    VkMemoryPropertyFlags properties;

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = rawTexture->numLevels;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = rawTexture->numLayers;

    uint32_t regionsNumber = rawTexture->numLevels;
    VkBufferImageCopy* regions = new VkBufferImageCopy[regionsNumber];

    size_t textureSize = ktxTexture_GetDataSizeUncompressed((ktxTexture*)rawTexture);

    properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = textureSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.pNext = nullptr;

    vkr = vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    assert(vkr == VK_SUCCESS);


    vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

    allocInfo = {};

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
    allocInfo.allocationSize = memRequirements.size;

    vkr = vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
    assert(vkr == VK_SUCCESS);

    vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

    ktx_uint8_t* pMappedStagingBuffer;
    vkr = vkMapMemory(device, stagingBufferMemory, 0, textureSize, 0, (void**)&pMappedStagingBuffer);
    assert(vkr == VK_SUCCESS);

    user_cbdata_optimal cbData;
    cbData.offset = 0;
    cbData.region = regions;
    cbData.numFaces = rawTexture->numFaces;
    cbData.numLayers = rawTexture->numLayers;
    cbData.dest = pMappedStagingBuffer;
    cbData.elementSize = ktxTexture_GetElementSize((ktxTexture*)rawTexture);
    cbData.numDimensions = rawTexture->numDimensions;
#if defined(_DEBUG)
    cbData.regionsArrayEnd = regions + regionsNumber;
#endif

    assert(rawTexture->dataSize <= allocInfo.allocationSize);
    memcpy(pMappedStagingBuffer, rawTexture->pData, rawTexture->dataSize);
    ktxr = rawTexture->vtbl->IterateLevels((ktxTexture*)rawTexture, [](int miplevel, int face,
        int width, int height, int depth,
        ktx_uint64_t faceLodSize,
        void* pixels, void* userdata) -> ktx_error_code_e
        {
            user_cbdata_optimal* ud = (user_cbdata_optimal*)userdata;
            (void)(pixels);

            // Set up copy to destination region in final image
#if defined(_DEBUG)
            assert(ud->region < ud->regionsArrayEnd);
#endif
            ud->region->bufferOffset = ud->offset;
            ud->offset += faceLodSize;
            // These 2 are expressed in texels.
            ud->region->bufferRowLength = 0;
            ud->region->bufferImageHeight = 0;
            ud->region->imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ud->region->imageSubresource.mipLevel = miplevel;
            ud->region->imageSubresource.baseArrayLayer = face;
            ud->region->imageSubresource.layerCount = ud->numLayers * ud->numFaces;
            ud->region->imageOffset.x = 0;
            ud->region->imageOffset.y = 0;
            ud->region->imageOffset.z = 0;
            ud->region->imageExtent.width = width;
            ud->region->imageExtent.height = height;
            ud->region->imageExtent.depth = depth;

            ud->region += 1;

            return KTX_SUCCESS;
        }, &cbData);

    vkUnmapMemory(device, stagingBufferMemory);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange = subresourceRange;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkCmdCopyBufferToImage(copyCommandBuffer, stagingBuffer, m_Resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regionsNumber, regions);

    barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Resource;
    barrier.subresourceRange = subresourceRange;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(
        copyCommandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkEndCommandBuffer(copyCommandBuffer);

    (*copyPipeline)->Wait();

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
    delete[] regions;
}

void SampleRenderV2::VKTexture2D::TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Transition for transferring data (copy buffer -> image)
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Transition for shader access (after data has been copied)
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::runtime_error("Unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
}

uint32_t SampleRenderV2::VKTexture2D::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    auto adapter = (*m_Context)->GetAdapter();
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(adapter, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return 0xffffffff;
}

VkImageType SampleRenderV2::VKTexture2D::GetNativeTensor(TextureTensor tensor)
{
    switch (tensor)
    {
    case TextureTensor::TENSOR_1:
        return VK_IMAGE_TYPE_1D;
    case TextureTensor::TENSOR_2:
        return VK_IMAGE_TYPE_2D;
    case TextureTensor::TENSOR_3:
        return VK_IMAGE_TYPE_3D;
    default:
        return VK_IMAGE_TYPE_MAX_ENUM;
    }
}

VkImageViewType SampleRenderV2::VKTexture2D::GetNativeTensorView(TextureTensor tensor)
{
    switch (tensor)
    {
    case TextureTensor::TENSOR_1:
        return VK_IMAGE_VIEW_TYPE_1D;
    case TextureTensor::TENSOR_2:
        return VK_IMAGE_VIEW_TYPE_2D;
    case TextureTensor::TENSOR_3:
        return VK_IMAGE_VIEW_TYPE_3D;
    default:
        return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}
