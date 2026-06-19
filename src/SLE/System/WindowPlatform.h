#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>


const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;


class VulkanPlatform;

class WindowPlatform
{
public:
	WindowPlatform() = default;

	/**
	* Destructeur de la classe WindowPlatform.
	*
	* @details
	* Appelle la méthode `Cleanup()` pour libérer les ressources allouées par la fenêtre GLFW.
	* Cela garantit que la fenêtre et les ressources associées sont correctement nettoyées lorsque l'objet `WindowPlatform` est détruit.
	*/
	~WindowPlatform();

	WindowPlatform(const WindowPlatform&) = delete;
	WindowPlatform& operator=(const WindowPlatform&) = delete;

	/**
	* Retourne le pointeur vers la fenêtre GLFW.
	* @return Pointeur vers la fenêtre GLFW.
	*/
	GLFWwindow* GetWindow() { return m_Window; }

	bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
	VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
	bool WasWindowResized() { return m_FramebufferResized; }
	void ResetWindowResizedFlag() { m_FramebufferResized = false; }

	/**
	* Initialise la fenêtre GLFW.
	*
	* @details
	* - **Initialisation de GLFW** : Appelle `glfwInit()` pour initialiser la bibliothèque GLFW.
	* - **Configuration de la fenêtre** :
	*   - Définit l'hint `GLFW_CLIENT_API` à `GLFW_NO_API` pour indiquer que la fenêtre ne sera pas utilisée avec OpenGL, mais avec Vulkan.
	*   - Crée une fenêtre de taille `WIDTH` x `HEIGHT` avec le titre "Vulkan" en utilisant `glfwCreateWindow`.
	*     - Les paramètres `nullptr` pour le moniteur et le partage de contexte indiquent que la fenêtre sera créée en mode fenêtre (non plein écran) et sans partage de contexte OpenGL.
	* - **Configuration des callbacks** :
	*   - Définit le pointeur utilisateur de la fenêtre (`glfwSetWindowUserPointer`) à `this` (l'instance actuelle de `WindowPlatform`). Cela permet de récupérer l'instance de `WindowPlatform` dans les callbacks GLFW.
	*   - Définit le callback de redimensionnement du framebuffer (`glfwSetFramebufferSizeCallback`) à `FrameBufferResizeCallback`. Ce callback sera appelé chaque fois que la taille du framebuffer de la fenêtre change.
	*/
	void InitWindow(int _Width, int _Height, std::string _Name);

	/**
	* Nettoie les ressources de la fenêtre GLFW.
	*
	* @details
	* - **Destruction de la fenêtre** : Appelle `glfwDestroyWindow` pour détruire la fenêtre GLFW (`m_Window`).
	* - **Terminaison de GLFW** : Appelle `glfwTerminate()` pour libérer toutes les ressources allouées par GLFW.
	* - Cette méthode est appelée dans le destructeur pour garantir que les ressources sont libérées lorsque l'objet `WindowPlatform` est détruit.
	*
	* @note
	* - Après l'appel à `Cleanup`, `m_Window` devient invalide. Toute tentative d'utiliser `m_Window` après cela entraînera un comportement indéfini.
	* - Si `m_Window` est déjà `nullptr`, `glfwDestroyWindow` ne fera rien.
	*/
	void Cleanup();

	void CreateWindowSurface(VkInstance _Instance, VkSurfaceKHR* Surface);

private:

	GLFWwindow* m_Window;

	int m_Width;
	int m_Height;
	bool m_FramebufferResized = false;

	/**
	* Callback appelé lorsque la taille du framebuffer de la fenêtre change.
	* @param _Window Pointeur vers la fenêtre GLFW dont le framebuffer a été redimensionné.
	* @param _Width Nouvelle largeur du framebuffer.
	* @param _Heigth Nouvelle hauteur du framebuffer.
	*
	* @details
	* - **Récupération de l'instance de WindowPlatform** :
	*   - Utilise `glfwGetWindowUserPointer` pour récupérer le pointeur utilisateur de la fenêtre (`this`), qui a été défini dans `InitWindow`.
	*   - Le pointeur est casté en `WindowPlatform*` avec `reinterpret_cast`.
	* - **Notification à VulkanPlatform** :
	*   - Si `app->m_VulkanPlatform` n'est pas `nullptr`, appelle `SetFrameBufferResized(true)` sur la plateforme Vulkan associée.
	*   - Cela permet de notifier la plateforme Vulkan que le framebuffer a été redimensionné, afin qu'elle puisse recréer les ressources nécessaires (comme le swap chain).
	*/
	static void FrameBufferResizeCallback(GLFWwindow* _Window, int _Width, int _Heigth);
};