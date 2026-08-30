#pragma once
#include "../Core/IVulkanDescription.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "../Common/Vector.h"

#include <array>
#include <vector>

struct Vertex : public IVulkanDescription
{
public:

	glm::vec3 Position{};
	glm::vec3 Color{};
	glm::vec3 Normal{};
	glm::vec2 UV{};

	static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{1};
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, UV);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& _Other) const
	{
		return (Position == _Other.Position) && (Color == _Other.Color) && (Normal == _Other.Normal) && (UV == _Other.UV);
	}
};

// Basic _Vertex with position only
struct BasicVertex : public IVulkanDescription 
{
    glm::vec3 Position;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(BasicVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() 
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(BasicVertex, Position);

        return attributeDescriptions;
    }

    bool operator==(const BasicVertex& _Other) const 
    {
        return Position == _Other.Position;
    }
};

// Vertex with position and Color
struct ColoredVertex : public IVulkanDescription
{
    glm::vec3 Position;
    glm::vec3 Color;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(ColoredVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() 
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(ColoredVertex, Position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(ColoredVertex, Color);

        return attributeDescriptions;
    }

    bool operator==(const ColoredVertex& _Other) const 
    {
        return Position == _Other.Position && Color == _Other.Color;
    }
};

// Standard _Vertex with position, Color, and texture coordinates (your current Vertex)
struct StandardVertex 
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 UV;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(StandardVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return { bindingDescription };
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() 
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(StandardVertex, Position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(StandardVertex, Color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(StandardVertex, UV);

        return attributeDescriptions;
    }

    bool operator==(const StandardVertex& _Other) const
    {
        return Position == _Other.Position && Color == _Other.Color && UV == _Other.UV;
    }
};


namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& _Vertex) const
		{
			return ((hash<glm::vec3>()(_Vertex.Position) ^ (hash<glm::vec3>()(_Vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(_Vertex.UV) << 1);
		}
	};

    template<> struct hash<BasicVertex> 
    {
        size_t operator()(BasicVertex const& _Vertex) const 
        {
            return hash<glm::vec3>()(_Vertex.Position);
        }
    };

    template<> struct hash<ColoredVertex> 
    {
        size_t operator()(ColoredVertex const& _Vertex) const 
        {
            return ((hash<glm::vec3>()(_Vertex.Position) ^ (hash<glm::vec3>()(_Vertex.Color) << 1)));
        }
    };

    template<> struct hash<StandardVertex> 
    {
        size_t operator()(StandardVertex const& _Vertex) const 
        {
            return ((hash<glm::vec3>()(_Vertex.Position) ^ (hash<glm::vec3>()(_Vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(_Vertex.UV) << 1);
        }
    };
}
