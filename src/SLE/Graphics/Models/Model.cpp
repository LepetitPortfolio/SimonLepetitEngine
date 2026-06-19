#include "Model.h"
#include "../../Core/GlobalFunctionLibrary.h"
#include "../../System/VulkanPlatform.h"
#include "../../Common/Error.h"

#include <functional>



Model::Model()
{
}

Model::Model(Texture* _Texture, Shader* _ShaderProgram)
{
	SetTexture(_Texture);
	SetProgram(_ShaderProgram);
}

Model::~Model()
{
	m_Vertices.clear();
	m_Indices.clear();
	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), m_VertexBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_VertexBufferMemory, nullptr);
	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), m_IndexBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_IndexBufferMemory, nullptr);

	m_Texture = nullptr;
	m_ShaderProgram = nullptr;
}

std::vector<Vertex>& Model::GetVertices()
{
	return m_Vertices;
}

std::vector<uint32_t>& Model::GetIndices()
{
	return m_Indices;
}

VkBuffer& Model::GetVertexBuffer()
{
	return m_VertexBuffer;
}

VkDeviceMemory& Model::GetVertexBufferMemory()
{
	return m_VertexBufferMemory;
}

VkBuffer& Model::GetIndexBuffer()
{
	return m_IndexBuffer;
}

VkDeviceMemory& Model::GetIndexBufferMemory()
{
	return m_IndexBufferMemory;
}


void Model::ShowInGame(bool _Value)
{
	if (_Value)
	{
		GlobalFunctionLibrary::GetVulkanPlatform()->GetDrawDelegate() += std::bind(&Model::Draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		//m_StartTime.Reset();
	}
	else
	{
		GlobalFunctionLibrary::GetVulkanPlatform()->GetDrawDelegate() -= std::bind(&Model::Draw, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		//m_StartTime.Stop();
	}
}

void Model::CreateModel()
{
}

void Model::Draw(VulkanData& _VulkanData, VkCommandBuffer& _CommandBuffer, uint32_t _ImageIndex)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(_CommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
		Err() << "failed to begin recording command buffer!" << std::endl;
	}

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = _VulkanData.RenderPass;
	renderPassBeginInfo.framebuffer = _VulkanData.SwapChainFramebuffers[_ImageIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = _VulkanData.SwapChainExtent;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	if (m_ShaderProgram)
	{
		vkCmdBindPipeline(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ShaderProgram->GetPipeline());
	}

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)_VulkanData.SwapChainExtent.width;
	viewport.height = (float)_VulkanData.SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(_CommandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = _VulkanData.SwapChainExtent;
	vkCmdSetScissor(_CommandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(_CommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(_CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);

	if (m_ShaderProgram)
	{
		vkCmdBindDescriptorSets(_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ShaderProgram->GetPipelineLayout(), 0, 1, &m_DescriptorSets[GlobalFunctionLibrary::GetVulkanPlatform()->GetCurrentFrameIndex()], 0, nullptr);
	}

	
	vkCmdDrawIndexed(_CommandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(_CommandBuffer);

	if (vkEndCommandBuffer(_CommandBuffer) != VK_SUCCESS) {
		Err() << "failed to record command buffer!" << std::endl;
	}
}

void Model::SetProgram(Shader* _ShaderProgram)
{
	if (_ShaderProgram == nullptr)
	{
		Err() << "Shader Program is null!" << std::endl;
		return;
	}

	std::vector<VkDescriptorSetLayout> setLayouts(MAX_FRAMES_IN_FLIGHT, _ShaderProgram->GetDescriptorSetLayout());
	m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorSetAllocateInfo setAllocateInfo{};
	setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocateInfo.descriptorPool = GlobalFunctionLibrary::GetVulkanData()->DescriptorPool;
	setAllocateInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	setAllocateInfo.pSetLayouts = setLayouts.data();

	if (vkAllocateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), &setAllocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
	{
		Err() << "failed to allocate descriptor sets!" << std::endl;
	}

	m_ShaderProgram = _ShaderProgram;
	UpdateDescriptorSets();
}

void Model::Destroy()
{
	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), m_IndexBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_IndexBufferMemory, nullptr);

	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), m_VertexBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), m_VertexBufferMemory, nullptr);
}

void Model::SetTexture(Texture* _Texture)
{
	if(!_Texture)
	{
		Err() << "Texture is null!" << std::endl;
		return;
	}
	m_Texture = _Texture;
	UpdateDescriptorSets();
}

const Texture* Model::GetTexture() const
{
	return m_Texture;
}

void Model::AddVertex(Vertex _Vertex)
{
	m_Vertices.push_back(_Vertex);
}

void Model::AddIndex(uint32_t _Index)
{
	m_Indices.push_back(_Index);
}

void Model::UpdateDescriptorSets()
{
	if(m_ShaderProgram == nullptr)
	{
		return;
	}

	for (size_t frameIndex = 0; frameIndex < MAX_FRAMES_IN_FLIGHT; frameIndex++)
	{
		std::vector<VkDescriptorSetLayoutBinding>* layoutBinding = m_ShaderProgram->GetLayoutBinding();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets{};

		for (size_t layoutBindingIndex = 0; layoutBindingIndex < layoutBinding->size(); layoutBindingIndex++)
		{
			VkDescriptorSetLayoutBinding lBinding = layoutBinding->at(layoutBindingIndex);
			
			switch (lBinding.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:

				m_ShaderProgram->GenertateUniformBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], frameIndex, writeDescriptorSets);
				
			break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:

				m_ShaderProgram->GenertateCombinedImageSamplerDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], m_Texture, writeDescriptorSets);
			break;

			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:

				//m_ShaderProgram->GenertateStorageBufferDescriptorSetLayout(lBinding, m_DescriptorSets[frameIndex], writeDescriptorSets);
				
			break;

			};
		}

		vkUpdateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void Model::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(GetVertices()[0]) * GetVertices().size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, GetVertices().data(), (size_t)bufferSize);
	vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory);

	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, GetVertexBuffer(), GetVertexBufferMemory());

	VulkanPlatform::CopyBuffer(stagingBuffer, GetVertexBuffer(), bufferSize);

	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, nullptr);
}

void Model::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(GetIndices()[0]) * GetIndices().size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, GetIndices().data(), (size_t)bufferSize);
	vkUnmapMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory);
	VulkanPlatform::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, GetIndexBuffer(), GetIndexBufferMemory());

	VulkanPlatform::CopyBuffer(stagingBuffer, GetIndexBuffer(), bufferSize);
	vkDestroyBuffer(GlobalFunctionLibrary::GetVulkanDevice(), stagingBuffer, nullptr);
	vkFreeMemory(GlobalFunctionLibrary::GetVulkanDevice(), stagingBufferMemory, nullptr);
}