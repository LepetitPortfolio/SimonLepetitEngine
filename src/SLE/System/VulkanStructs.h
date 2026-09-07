
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLFORCE_RADIANS
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include "../Common/Vertex.h"
#include "../Graphics/Textures/TextureVoid.h"

#include <optional>
#include <string>
#include <vector>

struct Config 
{
	uint32_t WindowWidth = 1600;
	uint32_t WindowHeight = 900;
	std::string WindowTitle = "SLE";
	uint32_t MaxFramesInFlight = 2;
	bool EnableValidation = true;
	bool EnableGui = true;
	std::string FontPath = "";
	float FontSize = 16.0f;
};

/**
 * Stocke les indices des familles de files d'attente (queue families) pour le rendu et la présentation.
 *
 * En Vulkan, une "queue family" est un ensemble de files d'attente qui partagent les mêmes capacités.
 * Par exemple, une famille peut supporter le rendu graphique, tandis qu'une autre peut supporter la présentation à l'écran.
 */
struct QueueFamilyIndices
{
public:
	/**
	 * Index de la famille de files d'attente qui supporte les opérations graphiques (rendu).
	 * @details
	 * - `std::optional<uint32_t>` permet de représenter un index qui peut être absent (si aucune famille ne supporte le rendu graphique).
	 * - Utilisé pour les commandes de dessin (ex: `vkCmdDraw`).
	 */
	std::optional<uint32_t> GraphicsFamily;

	/**
	 * Index de la famille de files d'attente qui supporte la présentation (affichage à l'écran).
	 * @details
	 * - `std::optional<uint32_t>` permet de représenter un index qui peut être absent (si aucune famille ne supporte la présentation).
	 * - Utilisé pour présenter les images rendues à la surface de l'écran (ex: via `vkQueuePresentKHR`).
	 */
	std::optional<uint32_t> PresentFamily;

	/**
	* Vérifie si les indices des familles de files d'attente sont complets.
	* @return true si les deux indices (GraphicsFamily et PresentFamily) sont définis, false sinon.
	*
	* @details
	* - Utilise `has_value()` pour vérifier si `GraphicsFamily` et `PresentFamily` contiennent une valeur.
	* - Une instance de `QueueFamilyIndices` est considérée comme "complète" si elle a à la fois un index pour le rendu et un index pour la présentation.
	*/
	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

/**
 * Stocke les détails de support du swap chain pour une surface Vulkan.
 *
 * Ces détails sont utilisés pour configurer le swap chain, qui est responsable de la présentation des images rendues à l'écran.
 */
struct SwapChainSupportDetails
{
public:
	/**
	 * Capacités de la surface (ex: taille minimale/maximale des images, nombre d'images, etc.).
	 * @details
	 * - `VkSurfaceCapabilitiesKHR` contient des informations comme :
	 *   - `minImageCount` : Nombre minimal d'images dans le swap chain.
	 *   - `maxImageCount` : Nombre maximal d'images dans le swap chain (0 = pas de limite).
	 *   - `currentExtent` : Taille actuelle de la surface (en pixels).
	 *   - `minImageExtent` / `maxImageExtent` : Tailles minimale et maximale des images.
	 *   - `supportedUsageFlags` : Utilisations supportées pour les images du swap chain (ex: `VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT`).
	 */
	VkSurfaceCapabilitiesKHR Capabilities;

	/**
	 * Liste des formats de surface supportés (ex: RGBA8, BGRA8, etc.).
	 * @details
	 * - `std::vector<VkSurfaceFormatKHR>` contient les formats disponibles pour les images du swap chain.
	 * - Chaque `VkSurfaceFormatKHR` contient :
	 *   - `format` : Format des pixels (ex: `VK_FORMAT_B8G8R8A8_SRGB`).
	 *   - `colorSpace` : Espace colorimétrique (ex: `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR`).
	 */
	std::vector<VkSurfaceFormatKHR> Formats;

	/**
	 * Liste des modes de présentation supportés (ex: FIFO, MAILBOX, etc.).
	 * @details
	 * - `std::vector<VkPresentModeKHR>` contient les modes de présentation disponibles.
	 * - Exemples de modes :
	 *   - `VK_PRESENT_MODE_IMMEDIATE_KHR` : Présentation immédiate (peut causer des déchirures).
	 *   - `VK_PRESENT_MODE_FIFO_KHR` : Présentation en file d'attente (VSYNC, pas de déchirure).
	 *   - `VK_PRESENT_MODE_MAILBOX_KHR` : Présentation en mode "boîte aux lettres" (triple buffering, pas de déchirure).
	 *   - `VK_PRESENT_MODE_RELAXED_FIFO_KHR` : Similaire à FIFO, mais peut sauter des images si l'application est en retard.
	 */
	std::vector<VkPresentModeKHR> PresentModes;
};

/**
 * Stocke les matrices de transformation pour le rendu 3D.
 *
 * Les UBOs sont utilisés pour passer des données uniformes (comme les matrices de transformation) aux shaders.
 * L'alignement (`alignas`) est nécessaire pour respecter les exigences d'alignement du GPU.
 */
struct UniformBufferObject
{
public:

	alignas(16) glm::mat4 Model;

	/**
	 * Matrice de projection (transformation de la vue 3D en 2D pour l'écran).
	 * @details
	 * - `glm::mat4` représente la projection (perspective ou orthographique).
	 * - `alignas(16)` garantit l'alignement sur 16 octets.
	 */
	alignas(16) glm::mat4 Projection;

	/**
	 * Matrice de vue (transformation de la caméra dans l'espace monde).
	 * @details
	 * - `glm::mat4` représente la position et l'orientation de la caméra.
	 * - `alignas(16)` garantit l'alignement sur 16 octets.
	 */
	alignas(16) glm::mat4 View;

	alignas(16) glm::mat4 InverseView;	
};


struct VulkanData
{

public:
	// --------------------------------------------------------------------
	// Messager de débogage Vulkan (extension EXT) : permet de recevoir des notifications de validation,
	// d'erreurs ou d'avertissements de l'API Vulkan pendant le développement.
	VkDebugUtilsMessengerEXT DebugMessenger;

	// --------------------------------------------------------------------
	// Surface Vulkan : interface entre la fenêtre système (ex: GLFW) et Vulkan.
	// Représente la zone où les images seront affichées.
	VkSurfaceKHR Surface;


	// --------------------------------------------------------------------
	// Nombre d'échantillons pour le multisampling (MSAA) : améliore la qualité visuelle en réduisant les aliasing.
	// Initialisé à VK_SAMPLE_COUNT_1_BIT (pas de MSAA par défaut).
	VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	//TextureVoid* DepthTexture = nullptr;
	std::vector<TextureVoid*> DepthTextures;

	std::vector<VkSemaphore> ImageAvailableSemaphores{};

	std::vector<VkSemaphore> RenderFinishedSemaphores{};

	std::vector<VkFence> InFlightFences{};

	uint32_t CurrentFrameIndexInFlight = 0;

	VulkanData() = default;

	//VulkanData(const VulkanData&) = delete;
	//VulkanData& operator=(const VulkanData&) = delete;
};

struct VulkanFrameInfo
{
public:
	uint32_t FrameIndex;
	uint32_t CurrentFrameIndexInFlight;
	float FrameTime;
	VkCommandBuffer CommandBuffer;
	class CameraBase* Camera;
};

struct PipelineConfigInfo 
{
public:
	std::vector<VkVertexInputBindingDescription> BindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> AttributeDescriptions{};
	VkPipelineViewportStateCreateInfo ViewportInfo;
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo RasterizationInfo;
	VkPipelineMultisampleStateCreateInfo MultisampleInfo;
	VkPipelineColorBlendAttachmentState ColorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
	std::vector<VkDynamicState> DynamicStateEnables;
	VkPipelineDynamicStateCreateInfo DynamicStateInfo;
	VkPipelineLayout PipelineLayout = nullptr;
	VkRenderPass RenderPass = nullptr;
	uint32_t Subpass = 0;

	PipelineConfigInfo() = default;
	//PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	//PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
};