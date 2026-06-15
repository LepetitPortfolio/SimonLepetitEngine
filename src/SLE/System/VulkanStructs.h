
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLFORCE_RADIANS
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

#include "../Graphics/Vertex.h"

#include <optional>
#include <vector>

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};

struct VulkanData
{
	// --------------------------------------------------------------------
	// Instance Vulkan : objet racine qui représente la connexion à une implémentation Vulkan.
	// Doit être créée en premier et détruite en dernier. Gère les propriétés globales de l'application.
	VkInstance Instance;

	// --------------------------------------------------------------------
	// Messager de débogage Vulkan (extension EXT) : permet de recevoir des notifications de validation,
	// d'erreurs ou d'avertissements de l'API Vulkan pendant le développement.
	VkDebugUtilsMessengerEXT DebugMessenger;

	// --------------------------------------------------------------------
	// Surface Vulkan : interface entre la fenêtre système (ex: GLFW) et Vulkan.
	// Représente la zone où les images seront affichées.
	VkSurfaceKHR Surface;

	// --------------------------------------------------------------------
	// Device physique (GPU) : représente le matériel graphique sélectionné pour le rendu.
	// Initialisé à VK_NULL_HANDLE (aucune valeur valide).
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

	// --------------------------------------------------------------------
	// Nombre d'échantillons pour le multisampling (MSAA) : améliore la qualité visuelle en réduisant les aliasing.
	// Initialisé à VK_SAMPLE_COUNT_1_BIT (pas de MSAA par défaut).
	VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	// --------------------------------------------------------------------
	// Device logique : interface pour interagir avec le device physique.
	// Représente la connexion logique au GPU, utilisée pour créer la plupart des objets Vulkan (buffers, images, etc.).
	VkDevice Device;

	// --------------------------------------------------------------------
	// File d'attente graphique : utilisée pour soumettre des commandes de rendu (ex: dessin, calculs).
	// Les commandes soumises ici sont exécutées par le GPU.
	VkQueue GraphicsQueue;

	VkQueue ComputeQueue;

	// --------------------------------------------------------------------
	// File d'attente de présentation : utilisée pour présenter les images rendues à l'écran (via la swap chain).
	// Peut être la même que GraphicsQueue si le GPU le permet.
	VkQueue PresentQueue;

	// --------------------------------------------------------------------
	// Swap chain : série d'images utilisées pour l'affichage.
	// Gère l'échange entre les images rendues par le GPU et celles affichées à l'écran.
	VkSwapchainKHR SwapChain;

	// --------------------------------------------------------------------
	// Liste des images appartenant à la swap chain.
	// Chaque image peut être rendue puis présentée à l'écran.
	std::vector<VkImage> SwapChainImages;

	// --------------------------------------------------------------------
	// Format des images de la swap chain (ex: VK_FORMAT_B8G8R8A8_SRGB pour RGBA 8 bits par canal).
	// Détermine comment les couleurs sont stockées dans les images.
	VkFormat SwapChainImageFormat;

	// --------------------------------------------------------------------
	// Étendue (largeur et hauteur) des images de la swap chain, en pixels.
	// Doit correspondre à la résolution de la fenêtre (ou être ajustée).
	VkExtent2D SwapChainExtent;

	// --------------------------------------------------------------------
	// Liste des vues d'images pour chaque image de la swap chain.
	// Une vue d'image permet d'accéder à une image (ou une partie) dans les shaders.
	std::vector<VkImageView> SwapChainImageViews;

	// --------------------------------------------------------------------
	// Liste des framebuffers, un pour chaque image de la swap chain.
	// Un framebuffer est une collection d'attaches (couleur, profondeur, etc.) utilisées pour le rendu.
	std::vector<VkFramebuffer> SwapChainFramebuffers;

	// --------------------------------------------------------------------
	// Render pass : définit comment les attaches (couleur, profondeur, etc.) sont utilisées pendant le rendu.
	// Décrit les étapes de rendu (ex: effacement, rendu des triangles, résolution du MSAA).
	VkRenderPass RenderPass;

	// --------------------------------------------------------------------
	// Pool de commandes : réservoir de mémoire pour allouer des buffers de commandes.
	// Les buffers de commandes stockent les commandes de rendu (ex: vkCmdDraw) à soumettre au GPU.
	VkCommandPool CommandPool;

	// --------------------------------------------------------------------
	// Image de couleur multi-échantillonnée (MSAA) : utilisée comme attache de couleur dans le render pass.
	// Stocke les données de couleur avec un échantillonnage multiple pour réduire les aliasing.
	VkImage ColorImage;

	// --------------------------------------------------------------------
	// Mémoire allouée pour l'image de couleur MSAA.
	// Chaque image Vulkan nécessite de la mémoire explicitement allouée et liée.
	VkDeviceMemory ColorImageMemory;

	// --------------------------------------------------------------------
	// Vue de l'image de couleur MSAA : permet d'accéder à l'image dans les shaders.
	VkImageView ColorImageView;

	// --------------------------------------------------------------------
	// Image de profondeur : utilisée pour les tests de profondeur (z-buffer).
	// Stocke les informations de profondeur pour chaque pixel, permettant de masquer les objets cachés.
	VkImage DepthImage;

	// --------------------------------------------------------------------
	// Mémoire allouée pour l'image de profondeur.
	VkDeviceMemory DepthImageMemory;

	// --------------------------------------------------------------------
	// Vue de l'image de profondeur : permet d'accéder à l'image dans les shaders (ex: pour le shadow mapping).
	VkImageView DepthImageView;

	// --------------------------------------------------------------------
	// Liste des buffers uniformes, un pour chaque frame en vol (MAX_FRAMES_IN_FLIGHT).
	// Stockent des données globales (ex: matrices de transformation) accessibles par les shaders.
	std::vector<VkBuffer> UniformBuffers;

	// --------------------------------------------------------------------
	// Liste des mémoires allouées pour les buffers uniformes.
	// Chaque buffer uniformes nécessite sa propre mémoire.
	std::vector<VkDeviceMemory> UniformBuffersMemory;

	// --------------------------------------------------------------------
	// Liste des pointeurs vers la mémoire mappée des buffers uniformes.
	// Permet au CPU de mettre à jour directement le contenu des buffers uniformes sans appel Vulkan supplémentaire.
	// Chaque pointeur correspond à un buffer uniforme mappé en mémoire.
	std::vector<void*> UniformBufferMapped;

	// --------------------------------------------------------------------
	// Pool de descripteurs : réservoir de mémoire pour allouer des sets de descripteurs.
	// Les sets de descripteurs lient des ressources (buffers, textures) à des bindings dans les shaders.
	VkDescriptorPool DescriptorPool;

	// --------------------------------------------------------------------
	// Liste des buffers de commandes, un pour chaque frame en vol.
	// Chaque buffer stocke les commandes de rendu (ex: vkCmdDraw) pour une frame.
	// Permet d'enregistrer les commandes à l'avance et de les réutiliser.
	std::vector<VkCommandBuffer> CommandBuffers;

	std::vector<VkCommandBuffer> ComputeCommandBuffers;

	// --------------------------------------------------------------------
	// Liste des sémaphores pour signaler qu'une image de la swap chain est disponible pour le rendu.
	// Utilisés pour synchroniser l'acquisition d'une image avec le début du rendu.
	// Un sémaphore par frame en vol.
	std::vector<VkSemaphore> ImageAvailableSemaphores;

	// --------------------------------------------------------------------
	// Liste des sémaphores pour signaler que le rendu d'une frame est terminé.
	// Utilisés pour synchroniser la fin du rendu avec la présentation de l'image.
	// Un sémaphore par frame en vol.
	std::vector<VkSemaphore> RenderFinishedSemaphores;

	std::vector<VkSemaphore> ComputeFinishedSemaphores;

	// --------------------------------------------------------------------
	// Liste des clôtures (fences) pour synchroniser les frames en vol.
	// Une clôture par frame permet de s'assurer qu'une frame est terminée avant de commencer une nouvelle.
	// Empêche le CPU de soumettre trop de frames au GPU.
	std::vector<VkFence> InFlightFences;
};