#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../Core/Delegate/DelegateInclude.h"
#include "VulkanStructs.h"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <memory>
#include <vector>

const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

#if defined(NDEBUG) 
const bool EnableValidationLayers = true;
#elif defined(_DEBUG) 
const bool EnableValidationLayers = true;
#else
const bool EnableValidationLayers = false;
#endif * !NDEBUG

class WindowPlatform;

class VulkanPlatform
{
public:

	/**
	* Destructeur de la classe VulkanPlatform.
	* Appelle Cleanup() pour libérer toutes les ressources Vulkan allouées.
	*/
	~VulkanPlatform();

	static const VulkanData* GetVulkanData() { return m_VulkanData.get(); }

	DelegateMulticast<VulkanData&, VkCommandBuffer&, uint32_t>& GetDrawDelegate() { return m_DrawDelegate; }

	/**
	* Définit l'état du flag indiquant si le framebuffer a été redimensionné.
	* Met à jour m_FrameBufferResized uniquement si la nouvelle valeur est différente.
	*/
	void SetFrameBufferResized(bool _Value);

	const size_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

	/**
	* Initialise toutes les composantes Vulkan nécessaires pour le rendu.
	* Appelle séquentiellement les méthodes pour créer l'instance, la surface, le device physique/logique,
	* la swap chain, les pipelines, les buffers, les descripteurs, etc.
	* _WindowPlatform : Pointeur vers la plateforme de fenêtre associée.
	*/
	void InitVulkan(WindowPlatform* _WindowPlatform);

	/**
	* Dessine une frame : acquiert une image de la swap chain, enregistre les commandes de rendu,
	* soumet les commandes à la file d'attente graphique, et présente l'image à l'écran.
	* Gère également la synchronisation entre les frames et le redimensionnement de la fenêtre.
	*/
	void DrawFrame();

	/**
	* Nettoie toutes les ressources Vulkan allouées (pipeline, render pass, buffers, descripteurs, etc.).
	* Appelé lors de la destruction de l'objet VulkanPlatform.
	*/
	void Cleanup();

	/**
	* Attend que le device Vulkan soit inactif (toutes les commandes soumises sont terminées).
	*/
	void WaitIdle();

	/**
	* Crée un buffer Vulkan avec les paramètres spécifiés (taille, utilisation, propriétés de mémoire).
	* Alloue la mémoire et lie le buffer à cette mémoire.
	* _DeviceSize : Taille du buffer en octets.
	* _UsageFlags : Utilisation du buffer (ex: uniforme, vertex, index).
	* _MemoryPropertyFlags : Propriétés de la mémoire (ex: accessible par l'hôte, locale au device).
	* _Buffer, _BufferMemory : Références pour stocker le buffer et sa mémoire allouée.
	*/
	static void CreateBuffer(VkDeviceSize _DeviceSize, VkBufferUsageFlags _UsageFlags, VkMemoryPropertyFlags _MemoryPropertyFlags, VkBuffer& _Buffer, VkDeviceMemory& _BufferMemory);

	/**
	* Copie le contenu d'un buffer source vers un buffer destination.
	* Utilise une commande ponctuelle (single-time command) pour effectuer la copie de manière efficace.
	*/
	static void CopyBuffer(VkBuffer _SrcBuffer, VkBuffer _DstBuffer, VkDeviceSize _Size);

	/**
	* Commence l'enregistrement d'une commande ponctuelle (single-time command).
	* Les commandes ponctuelles sont utilisées pour des opérations comme la copie de buffers ou le changement de layout d'image.
	* Retourne le buffer de commandes alloué et prêt à l'enregistrement.
	*/
	static VkCommandBuffer BeginSingleTimeCommands();

	/**
	* Termine et soumet une commande ponctuelle, puis attend sa complétion.
	* Libère également le buffer de commandes.
	* _CommandBuffer : Buffer de commandes à finaliser.
	*/
	static void EndSingleTimeCommands(VkCommandBuffer _CommandBuffer);

	/**
	* Crée une vue d'image pour une image donnée.
	* Les vues d'images permettent d'accéder à une image ou à une partie d'une image dans les shaders.
	* _Image : Image pour laquelle créer la vue.
	* _Format : Format de l'image.
	* _AspectFlags : Aspects de l'image (ex: couleur, profondeur).
	* _MipLevels : Nombre de niveaux de mipmap accessibles via la vue.
	*/
	static VkImageView CreateImageView(VkImage _Image, VkFormat _Format, VkImageAspectFlags _AspectFlags, uint32_t _MipLevels);

	/**
	* Crée une image Vulkan avec les paramètres spécifiés (largeur, hauteur, niveaux de mipmap, échantillons, format, etc.).
	* Alloue également la mémoire nécessaire et lie l'image à cette mémoire.
	* _Width, _Heigth : Dimensions de l'image.
	* _MipLevels : Nombre de niveaux de mipmap.
	* _NumSamples : Nombre d'échantillons pour le multisampling.
	* _Format : Format de l'image (ex: VK_FORMAT_R8G8B8A8_SRGB).
	* _Tiling : Mode de disposition des pixels (OPTIMAL ou LINEAR).
	* _Usage : Utilisation de l'image (ex: couleur, profondeur, échantillonnage).
	* _Properties : Propriétés de la mémoire (ex: locale au device, accessible par l'hôte).
	* _Image, _ImageMemory : Références pour stocker l'image et sa mémoire allouée.
	*/
	static void CreateImage(uint32_t _Width, uint32_t _Heigth, uint32_t _MipLevels, VkSampleCountFlagBits _NumSample, VkFormat _Format, VkImageTiling _Tiling, VkImageUsageFlags  _Usage, VkMemoryPropertyFlags _Properties, VkImage& _Image, VkDeviceMemory& _ImageMemory);


private:

	WindowPlatform* m_WindowPlatform;

	static std::unique_ptr<VulkanData> m_VulkanData;

	size_t m_CurrentFrameIndex = 0;

	bool m_FrameBufferResized = false;

	DelegateMulticast<VulkanData&, VkCommandBuffer&, uint32_t> m_DrawDelegate;


	/**
	* Crée une instance Vulkan, structure de base pour toute application Vulkan.
	* En cas d'échec, affiche une erreur.
	* Configure les informations de l'application, les extensions requises (notamment pour GLFW),
	* et active les couches de validation si EnableValidationLayers est vrai.
	*/
	void CreateInstance();

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

	/**
	* Crée un device logique (interface pour interagir avec le device physique).
	* Configure les files d'attente pour le rendu et la présentation, active les fonctionnalités requises (comme l'anisotropie),
	* et charge les extensions nécessaires.
	*/
	void CreateLogicalDevice();

	/**
	* Crée une swap chain, une série d'images utilisées pour l'affichage.
	* Configure le format de surface, le mode de présentation (mailbox si disponible, sinon FIFO),
	* et l'étendue des images en fonction des capacités du device et de la fenêtre.
	*/
	void CreateSwapChain();

	/**
	* Interroge les capacités de la swap chain pour un device physique donné.
	* Récupère les formats de surface, les modes de présentation, et les capacités de la surface.
	*/
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _Device);

	/**
	* Sélectionne le format de surface optimal pour la swap chain.
	* Privilégie le format VK_FORMAT_B8G8R8A8_SRGB avec l'espace de couleur VK_COLOR_SPACE_SRGB_NONLINEAR_KHR.
	* Si non disponible, utilise le premier format disponible.
	*/
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& _AvailableFormats);

	/**
	* Sélectionne le mode de présentation optimal pour la swap chain.
	* Privilégie VK_PRESENT_MODE_MAILBOX_KHR (triple buffering) pour des performances optimales.
	* Si non disponible, utilise VK_PRESENT_MODE_FIFO_KHR (double buffering standard).
	*/
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& _AvailablePresentModes);

	/**
	* Détermine l'étendue (largeur/hauteur) des images de la swap chain.
	* Si l'étendue actuelle est définie (différente de la valeur maximale uint32_t), l'utilise directement.
	* Sinon, utilise les dimensions de la fenêtre et les limite aux capacités minimales/maximales du device.
	*/
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _Capabilities);

	/**
	* Crée des vues d'images pour chaque image de la swap chain.
	* Les vues d'images sont nécessaires pour accéder aux images de la swap chain dans les shaders.
	*/
	void CreateImageViews();

	/**
	* Crée un render pass, qui définit comment les attaches (couleur, profondeur, etc.) sont utilisées pendant le rendu.
	* Configure les attaches pour la couleur (avec MSAA), la profondeur, et la résolution (pour le MSAA).
	* Définit également les dépendances entre les sous-passages.
	*/
	void CreateRenderPass();

	/**
	* Crée un layout de set de descripteurs, qui définit comment les ressources (buffers, textures) sont accessibles dans les shaders.
	* Configure deux bindings : un pour les buffers uniformes (accès en vertex shader) et un pour les échantillonneurs de texture (accès en fragment shader).
	*/
	//void CreateDescriptorSetLayout();

	/**
	* Crée le pipeline graphique, qui définit comment les vertex sont transformés en pixels à l'écran.
	* Charge les shaders (vertex et fragment), configure l'assemblage des primitives, le viewport, le rasterization,
	* le multisampling, la profondeur/stencil, le blending des couleurs, et le layout du pipeline.
	* Les shaders sont chargés depuis des fichiers SPIR-V.
	*/
	//void CreateGraphicsPipeline();

	/**
	* Crée un module de shader à partir du code SPIR-V.
	* Les modules de shader sont utilisés pour charger les shaders dans le pipeline graphique.
	*/
	//VkShaderModule CreateShaderModule(const std::vector<char>& _ShaderCode);

	/**
	* Crée des framebuffers pour chaque vue d'image de la swap chain.
	* Un framebuffer est une collection d'attaches (couleur, profondeur, etc.) utilisées pour le rendu.
	* Ici, chaque framebuffer utilise une image de couleur MSAA, une image de profondeur, et une vue de la swap chain.
	*/
	void CreateFramebuffers();

	/**
	* Crée un pool de commandes, utilisé pour allouer des buffers de commandes.
	* Les buffers de commandes enregistrent les commandes de rendu (comme vkCmdDraw) qui sont ensuite soumises au GPU.
	*/
	void CreateCommandPool();

	/**
	* Crée une image de couleur multi-échantillonnée (MSAA) et sa vue associée.
	* Cette image est utilisée comme attache de couleur dans le render pass pour le MSAA.
	*/
	void CreateColorRessources();

	/**
	* Crée une image de profondeur et sa vue associée.
	* Cette image est utilisée comme attache de profondeur dans le render pass.
	*/
	void CreateDepthResources();

	/**
	* Trouve un format supporté par le device pour une utilisation donnée (ex: profondeur/stencil).
	* Vérifie les formats candidats et retourne le premier qui supporte les fonctionnalités requises (ex: VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT).
	*/
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& _Candidates, VkImageTiling _Tiling, VkFormatFeatureFlags _Features);

	/**
	* Trouve un format de profondeur supporté par le device.
	* Teste les formats courants (D32_SFLOAT, D32_SFLOAT_S8_UINT, D24_UNORM_S8_UINT) et retourne le premier supporté.
	*/
	VkFormat FindDepthFormat();

	/**
	* Vérifie si un format de profondeur inclut un composant stencil.
	* Retourne true pour les formats D32_SFLOAT_S8_UINT et D24_UNORM_S8_UINT.
	*/
	bool HasStencilComponent(VkFormat _Format);

	//void CreateTextureImage();


	//void GenerateMipMaps(VkImage _Image, VkFormat _ImageFormat, int32_t _TexWidth, int32_t _TexHeight, uint32_t _MipLevels);

	VkSampleCountFlagBits GetMaxUsableSampleCount();

	//void TransitionImageLayout(VkImage _Image, VkFormat _Format, VkImageLayout _OldLayout, VkImageLayout _NewLayout, uint32_t _MipLevels);

	//void CopyBufferToImage(VkBuffer _Buffer, VkImage _Image, uint32_t _Width, uint32_t _Height);

	//void CreateTextureImageView();

	//void CreateTextureSampler();


	//void CreateVertexBuffer();

	//void CreateIndexBuffer();

	/**
	* Crée des buffers uniformes pour chaque frame en vol (MAX_FRAMES_IN_FLIGHT).
	* Les buffers uniformes stockent des données (comme les matrices de transformation) accessibles par les shaders.
	* Ici, chaque buffer est mappé en mémoire pour permettre des mises à jour rapides par le CPU.
	*/
	void CreateUniformBuffers();

	/**
	* Trouve un type de mémoire compatible avec les exigences d'un buffer ou d'une image.
	* _TypeFilter : Masque des types de mémoire acceptables.
	* _MemoryProperty : Propriétés de mémoire requises (ex: accessible par l'hôte, locale au device).
	* Retourne l'index du type de mémoire trouvé.
	*/
	static uint32_t FindMemoryType(uint32_t _TypeFilter, VkMemoryPropertyFlags _MemoryProperty);

	/**
	* Crée un pool de descripteurs, utilisé pour allouer des sets de descripteurs.
	* Un pool de descripteurs est une réserve de mémoire pour les descripteurs (buffers uniformes, textures, etc.).
	*/
	void CreateDescriptorPool();

	/**
	* Crée des sets de descripteurs pour chaque frame en vol.
	* Un set de descripteurs lie des ressources (buffers, textures) à des bindings dans les shaders.
	* Ici, chaque set contient un buffer uniforme et un échantillonneur de texture.
	*/
	//void CreateDescriptorSets();

	/**
	* Crée des buffers de commandes pour chaque frame en vol.
	* Les buffers de commandes stockent les commandes de rendu (comme vkCmdDraw) pour chaque frame.
	*/
	void CreateCommandBuffers();

	/**
	* Crée des objets de synchronisation (sémaphores et clôtures) pour chaque frame en vol.
	* Les sémaphores sont utilisés pour synchroniser les opérations entre le CPU et le GPU.
	* Les clôtures (fences) permettent de s'assurer qu'une frame est terminée avant de commencer une nouvelle.
	*/
	void CreateSyncObjects();

	/**
	* Enregistre les commandes de rendu dans un buffer de commandes pour une image donnée.
	* Commence un render pass, lie le pipeline graphique, configure le viewport et la région de ciseaux,
	* lie les buffers de vertex et d'index, lie les sets de descripteurs, et soumet une commande de dessin indexé.
	* _CommandBuffer : Buffer de commandes dans lequel enregistrer les commandes.
	* _ImageIndex : Index de l'image de la swap chain à utiliser pour le rendu.
	*/
	//void RecordCommandBuffer(VkCommandBuffer _CommandBuffer, uint32_t _ImageIndex);

	/**
	* Met à jour le buffer uniforme pour la frame actuelle.
	* Calcule les matrices de modèle, vue, et projection, puis copie ces données dans le buffer uniforme mappé.
	* _CurrentImage : Index de l'image actuelle (non utilisé ici, mais pourrait l'être pour des animations par image).
	*/
	void UpdateUniformBuffer(uint32_t _CurrentImage);

	/**
	* Nettoie les ressources liées à la swap chain (images, vues, framebuffers, ressources de couleur et profondeur).
	* Appelé lors du redimensionnement de la fenêtre ou de la destruction de la swap chain.
	*/
	void CleanupSwapChain();

	/**
	* Re-crée la swap chain et ses ressources associées (images, vues, framebuffers, etc.).
	* Appelé lors du redimensionnement de la fenêtre ou si la swap chain devient incompatible.
	*/
	void RecreateSwapChain();

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
	* Callback de débogage Vulkan, appelée par le messager de débogage pour chaque message de validation.
	* Affiche les messages de sévérité "warning" ou supérieure dans la console d'erreur.
	* _MessageSeverity : Niveau de sévérité du message.
	* _MessageType : Type de message (général, validation, performance).
	* _pCallbackData : Données du message, incluant le message lui-même.
	* _pUserData : Données utilisateur (non utilisé ici).
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT _MessageType,
		const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData);

};