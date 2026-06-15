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
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = GlobalFunctionLibrary::GetVulkanData()->UniformBuffers[frameIndex];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		if (m_Texture)
		{
			imageInfo.imageView = m_Texture->GetTextureImageView();
			imageInfo.sampler = m_Texture->GetTextureSampler();
		}

		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = m_DescriptorSets[frameIndex];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &bufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = m_DescriptorSets[frameIndex];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(GlobalFunctionLibrary::GetVulkanDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}
