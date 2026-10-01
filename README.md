# SimonLepetitEngine (SLE)

Moteur de jeu 3D écrit en **C++17** et basé sur **Vulkan**, développé par Simon Lepetit dans un but d'apprentissage (architecture d'un moteur, rendu bas niveau, multi-plateforme Windows/Linux).

Le moteur charge et affiche des modèles OBJ texturés via un pipeline Vulkan complet (instance, device, swap chain, render pass, pipeline graphique, buffers, commandes), avec une couche de concepts gameplay (scènes, game objects, composants, caméras) et une gestion d'entrées (clavier, souris, manette).

## Fonctionnalités

- **Rendu Vulkan** : instance, device physique/logique, swap chain, pipeline graphique, buffers (vertex/index/UBO), command buffers, synchronisation.
- **Chargement d'assets** : modèles OBJ (`tinyobjloader`), textures (`stb_image`), shaders SPIR-V (`SPIRV-Reflect`).
- **Concepts gameplay** : `Engine` singleton, `SceneManager`, `Scene`, `GameObject`/composants, `CameraBase` avec caméra principale interchangeable.
- **Entrées** : `InputManager` avec modules clavier, souris et manette (gamepad).
- **Abstraction plateforme** : `WindowPlatform`/`VulkanPlatform`, utilitaires (`Clock`, `Time`, `Sleep`, `FileReader`, `String`, `UTF`, vecteurs/quaternions) avec implémentations Unix et Windows.
- **Build CMake** : presets Windows/Linux × Debug/Release (générateur Ninja), modules `Find*` pour dépendances, copie automatique des DLL Vulkan sur Windows.

## Prérequis

- Compilateur C++17 (MSVC, Clang ou GCC)
- [CMake](https://cmake.org/) ≥ 3.10 et [Ninja](https://ninja-build.org/)
- [Vulkan SDK](https://vulkan.lunarg.com/) (inclus `glslc`/`glslangValidator` pour compiler les shaders)

Les dépendances suivantes sont **vendées** dans `Libs/headers/` (pas d'installation nécessaire) : GLFW, GLM, stb, tinyobjloader, SPIRV-Reflect.

## Installation

```bash
git clone https://github.com/LepetitPortfolio/SimonLepetitEngine.git
cd SimonLepetitEngine
```

### Compiler les shaders (important)

L'exécutable charge `Shaders/Vert.spv` et `Shaders/Frag.spv`, **pas** les fichiers GLSL. Compiler avant de lancer :

```bash
cd Shaders
./ShaderCompile.bat   # Windows (utilise %VULKAN_SDK%)
```

### Configurer et compiler

```bash
cmake --preset Win-x64-release    # ou Linux-x64-release
cmake --build out/build/Win-x64-release
```

Presets disponibles : `Win-x64-debug/release`, `Win-x86-debug/release`, `Linux-x64-debug/release`, `Linux-x86-debug/release`.

### Lancer

L'exécutable `Game` est généré dans le dossier de build ; il doit être lancé depuis la racine du projet pour que les chemins relatifs (`Shaders/`, `Assets/`) soient résolus.

## Utilisation

`src/Game/Main.cpp` sert d'exemple minimal : il charge un shader vertex/fragment, une texture, un modèle (`viking_room.obj`) et l'ajoute à la scène courante avant de lancer la boucle principale du moteur.

```cpp
Engine* app = Engine::GetInstance();
Shader* shader = ShaderLoader::LoadVertexFragmentShader<EmptyVertex>("BasicShader", vertSpv, fragSpv);
Texture* texture = TextureLoader::LoadTexture(textureFile);
Model* model = ModelLoader::LoadModel(modelFile);
Mesh* mesh = new Mesh(model, shader, texture);
GlobalFunctionLibrary::GetCurrentScene()->AddGameObject(mesh);
app->MainLoop();
app->Cleanup();
```

## Architecture

```
SimonLepetitEngine/
├── CMakeLists.txt          # Racine : Vulkan + libs vendées + sous-projets
├── CMakePresets.json       # Presets Windows/Linux × Debug/Release
├── cmake/                  # Config, macros, warnings, modules Find* (DRM, EGL, GBM, UDev, GLES, Freetype, FLAC, Vorbis)
├── Libs/headers/           # Dépendances vendées : GLFW, GLM, stb, tinyobjloader, SPIRV-Reflect
├── Shaders/                # GLSL + ShaderCompile.bat + notes de diagnostic
├── ShadersBackup/          # Versions numérotées de shaders + SPIR-V précompilés
├── Assets/                 # Modèles OBJ/MTL et textures d'exemple
└── src/
    ├── Game/               # Exécutable d'exemple (Main.cpp)
    └── SLE/                # Bibliothèque du moteur
        ├── Engine          # Point d'entrée singleton : Init / MainLoop / Cleanup
        ├── Common/         # Clock, Time, Sleep, String, UTF, FileReader, Vector, Quaternion, Vertex, erreurs/exceptions
        ├── Core/           # Config, GameSetting, Transform, delegates, AssetData, bibliothèque de fonctions globales
        ├── Graphics/       # Mesh, ModelLoader, Shader(Loader), TextureLoader
        ├── GameplayConcepts/ # SceneManager, Scene, GameObject(+composants), CameraBase, Controler
        ├── Inputs/         # InputManager : Keyboard, Mouse, Gamepad
        └── System/         # Couche Vulkan (Instance, Device, SwapChain, Pipeline, Renderer, Buffer/Command managers), WindowPlatform
```

## Notes de développement

- `Shaders/README_DIAGNOSTIC_TRIANGLE.txt` décrit un shader de diagnostic (triangle rouge plein écran) pour isoler les pannes : si le triangle apparaît, le problème vient de la chaîne mesh/vertex/UBO/caméra ; sinon, il faut inspecter pipeline, render pass ou command buffers.
- `Shaders/README_FIXES.txt` liste les corrections déjà appliquées (UBO `Model/View/Projection/InverseView`, cohérence des attributs avec `StandardVertex`).
- Le build copie automatiquement les DLL du Vulkan SDK à côté de la bibliothèque sur Windows.

## Licence

Aucune licence n'est actuellement précisée dans le dépôt.

---
