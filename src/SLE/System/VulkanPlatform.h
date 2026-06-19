#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanStructs.h"

#include <memory>



class WindowPlatform;

class VulkanPlatform
{

public:

#if defined(NDEBUG) 
	const bool m_EnableValidationLayers = true;
#elif defined(_DEBUG) 
	const bool m_EnableValidationLayers = true;
#else
	const bool m_EnableValidationLayers = false;
#endif * !NDEBUG

	VulkanPlatform(WindowPlatform& _WindowPlatform);
	~VulkanPlatform();

	VulkanPlatform(const VulkanPlatform&) = delete;
	VulkanPlatform& operator=(const VulkanPlatform&) = delete;
	VulkanPlatform(VulkanPlatform&&) = delete;
	VulkanPlatform& operator=(VulkanPlatform&&) = delete;

	VulkanData* GetVulkanData() { return m_VulkanData.get(); }

	VkCommandPool GetCommandPool() { return m_VulkanData->CommandPool; }
	VkDevice GetDevice() { return m_VulkanData->Device; }
	VkSurfaceKHR GetSurface() { return m_VulkanData->Surface; }
	VkQueue GetGraphicsQueue() { return m_VulkanData->GraphicsQueue; }
	VkQueue GetPresentQueue() { return m_VulkanData->PresentQueue; }
	SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_VulkanData->PhysicalDevice); }
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_VulkanData->PhysicalDevice); }
	VkFormat FindSupportedFormat( const std::vector<VkFormat>& _Candidates, VkImageTiling _Tiling, VkFormatFeatureFlags _Features);

	/**
	* Crée un buffer Vulkan avec les paramètres spécifiés (taille, utilisation, propriétés de mémoire).
	* Alloue la mémoire et lie le buffer à cette mémoire.
	* _DeviceSize : Taille du buffer en octets.
	* _UsageFlags : Utilisation du buffer (ex: uniforme, vertex, index).
	* _MemoryPropertyFlags : Propriétés de la mémoire (ex: accessible par l'hôte, locale au device).
	* _Buffer, _BufferMemory : Références pour stocker le buffer et sa mémoire allouée.
	*/
	void CreateBuffer(VkDeviceSize _DeviceSize, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory);

	/**
	* Copie le contenu d'un buffer source vers un buffer destination.
	* Utilise une commande ponctuelle (single-time command) pour effectuer la copie de manière efficace.
	*/
	void CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size);

	/**
	* Commence l'enregistrement d'une commande ponctuelle (single-time command).
	* Les commandes ponctuelles sont utilisées pour des opérations comme la copie de buffers ou le changement de layout d'image.
	* Retourne le buffer de commandes alloué et prêt à l'enregistrement.
	*/
	VkCommandBuffer BeginSingleTimeCommands();
	
	/**
	* Termine et soumet une commande ponctuelle, puis attend sa complétion.
	* Libère également le buffer de commandes.
	* _CommandBuffer : Buffer de commandes à finaliser.
	*/
	void EndSingleTimeCommands(VkCommandBuffer _CommandBuffer);

	void CopyBufferToImage(	VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height, uint32_t _LayerCount);

	void CreateImageWithInfo( const VkImageCreateInfo& _ImageInfo, VkMemoryPropertyFlags _Properties, VkImage& _Image, VkDeviceMemory& _ImageMemory);

private:

	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	WindowPlatform& m_WindowPlatform;

	static std::unique_ptr<VulkanData> m_VulkanData;

	/**
	* Crée une instance Vulkan, structure de base pour toute application Vulkan.
	* En cas d'échec, affiche une erreur.
	* Configure les informations de l'application, les extensions requises (notamment pour GLFW),
	* et active les couches de validation si EnableValidationLayers est vrai.
	*/
	void CreateInstance();

	/**
	* Vérifie si les couches de validation Vulkan demandées sont supportées par l'instance.
	* Retourne true si toutes les couches sont disponibles, false sinon.
	*/
	bool CheckValidationLayerSupport();

	/**
	* Récupère la liste des extensions Vulkan requises pour l'application.
	* Inclut les extensions GLFW pour la création de surface, et l'extension de débogage si les couches de validation sont activées.
	*/
	std::vector<const char*> GetRequiredExtensions();

	/**
	* Configure le messager de débogage Vulkan pour recevoir des messages de validation.
	* Utilise PopulateDebugMessagerCreateInfo pour remplir les paramètres du messager.
	* Ne fait rien si EnableValidationLayers est faux.
	*/
	void SetupDebugMessenger();

	/**
	* Remplit une structure VkDebugUtilsMessengerCreateInfoEXT avec les paramètres de configuration du messager de débogage.
	* Définit les niveaux de sévérité (verbose, warning, error), les types de messages (général, validation, performance),
	* et associe la fonction de callback DebugCallback.
	*/
	void PopulateDebugMessagerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _MessengerCreateInfo);

	/**
	* Crée une surface Vulkan associée à la fenêtre GLFW.
	* La surface est nécessaire pour le rendu et l'affichage dans la fenêtre.
	*/
	void CreateSurface();

	/**
	* Sélectionne un device physique (GPU) compatible avec les exigences de l'application.
	* Enumère tous les devices disponibles et vérifie leur compatibilité avec IsDeviceSuitable.
	* Stocke le device sélectionné dans m_VulkanData->PhysicalDevice et définit le nombre d'échantillons MSAA maximal utilisable.
	*/
	void PickPhysicalDevice();

	/**
	* Vérifie si un device physique est adapté pour l'application.
	* Vérifie la disponibilité des familles de files d'attente (graphics et present),
	* le support des extensions requises, la compatibilité de la swap chain, et les fonctionnalités (comme l'anisotropie).
	* Retourne true si le device est compatible, false sinon.
	*/
	bool IsDeviceSuitable(VkPhysicalDevice _Device);

	/**
	* Interroge les capacités de la swap chain pour un device physique donné.
	* Récupère les formats de surface, les modes de présentation, et les capacités de la surface.
	*/
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _Device);

	/**
	* Vérifie si un device physique supporte toutes les extensions requises (DeviceExtensions).
	* Enumère les extensions disponibles et compare avec la liste des extensions requises.
	*/
	bool CheckDeviceExtensionSupport(VkPhysicalDevice _Device);

	/**
	* Note la pertinence d'un device physique en fonction de ses propriétés et fonctionnalités.
	* Attribue un score plus élevé aux GPUs discrets et aux devices supportant les shaders géométriques.
	* Retourne le score calculé.
	*/
	int RateDeviceSuitability(VkPhysicalDevice _Device);

	/**
	* Trouve les indices des familles de files d'attente (graphics et present) pour un device physique.
	* Les familles de files d'attente sont nécessaires pour soumettre des commandes de rendu et de présentation.
	*/
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice _Device);

	VkSampleCountFlagBits GetMaxUsableSampleCount();

	/**
	* Crée un device logique (interface pour interagir avec le device physique).
	* Configure les files d'attente pour le rendu et la présentation, active les fonctionnalités requises (comme l'anisotropie),
	* et charge les extensions nécessaires.
	*/
	void CreateLogicalDevice();

	/**
	* Crée un pool de commandes, utilisé pour allouer des buffers de commandes.
	* Les buffers de commandes enregistrent les commandes de rendu (comme vkCmdDraw) qui sont ensuite soumises au GPU.
	*/
	void CreateCommandPool();

};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT _MessageType, const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData)
{
	std::cerr << "validation layer: " << _pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}