#pragma once
#include "Engine.h"

#include "common/FileReader.h"
#include "common/String.h"
#include "Graphics/ShaderLoader.h"
#include "Graphics/TextureLoader.h"
#include "Graphics/ModelLoader.h"
#include "Graphics/Models/ModelInclude.h"
#include "Graphics/Mesh.h"
#include "Core/GlobalFunctionLibrary.h"
#include "GameplayConcepts/Scene.h"


int main() 
{
    Engine* app = Engine::GetInstance();

    if(app)
    {
        std::string vertexShader = FileReader::GetRootFolder().ToANSIString() + "Shaders/4_vert.spv";
        std::string fragmentShader = FileReader::GetRootFolder().ToANSIString() + "Shaders/4_frag.spv";
        std::string textureFile = FileReader::GetRootFolder().ToANSIString() + "Assets/Textures/viking_room.png";
        std::string earthTextureFile = FileReader::GetRootFolder().ToANSIString() + "Assets/Textures/2k_earth_daymap.jpg";
        std::string modelFile = FileReader::GetRootFolder().ToANSIString() + "Assets/Models/viking_room.obj";


		Shader* shader = ShaderLoader::LoadVertexFragmentShader<Vertex>("BasicShader", vertexShader.c_str(), fragmentShader.c_str());

        //Texture* texture = TextureLoader::LoadTexture(textureFile.c_str());
        Texture* texture = TextureLoader::LoadTexture(earthTextureFile.c_str());

        //Model* model = ModelLoader::LoadModel(modelFile.c_str(), texture, shader);
		SphereModel* sphere = new SphereModel(1.0f, 32, 32);

		Mesh* mesh = new Mesh(sphere, shader, texture);

        //GlobalFunctionLibrary::GetCurrentScene()->AddGameObject(model);
        GlobalFunctionLibrary::GetCurrentScene()->AddGameObject(mesh);


        app->MainLoop();
        app->Cleanup();
	}



    return 0;
}
