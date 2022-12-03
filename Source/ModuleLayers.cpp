#include "Headers.h"
#include "ModuleLayers.h"
#include "LayerEditor.h"
#include "LayerGame.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "TransformComponent.h"
#include "ModuleResourceManager.h"

#include "ModuleInput.h"

ModuleLayers::ModuleLayers()
{
}

ModuleLayers::~ModuleLayers()
{
}

bool ModuleLayers::Init()
{
    // Create Root GameObject (Scene)

    XMLNode sceneXML = Application::Instance()->xml->GetConfigXML();

    _sceneBeginPath = sceneXML.FindChildBreadth("currentScene").node.attribute("value").as_string();

    //if (!ModuleResourceManager::S_DeserializeScene(_sceneBeginPath))
    //{
    //    rootGameObject = new GameObject(nullptr, "Root", "None");
    //}
    rootGameObject = new GameObject(nullptr, "Root", "None");
    return true;
}

bool ModuleLayers::Start()
{
    layers[(uint)LayersID::EDITOR] = editor = new LayerEditor();
    layers[(uint)LayersID::GAME] = new LayerGame();

    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
        if (layers[i] && layers[i]->IsEnabled()) layers[i]->Start();
    }

    return true;
}

UpdateStatus ModuleLayers::PreUpdate()
{
    if(_requestScene)
    {
        if(!ModuleResourceManager::S_DeserializeScene(_requestScenePath))
        {
            _requestScenePath = "null";
        }

        _requestScene = false;
    }

    for (int i = 0; i < _deletedGameObjects.size(); i++)
    {
        RELEASE(_deletedGameObjects[i]);
    }
    _deletedGameObjects.clear();

    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
        if (layers[i] && layers[i]->IsEnabled()) layers[i]->PreUpdate();
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleLayers::Update()
{
    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
        if (layers[i] && layers[i]->IsEnabled()) layers[i]->Update();
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleLayers::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleLayers::DrawLayers()
{
    for (int i = 0; i < (uint)LayersID::EDITOR; i++)
    {
        if (layers[i] && layers[i]->IsEnabled()) layers[i]->PostUpdate();
    }
}

void ModuleLayers::DrawEditor()
{
    if (layers[(uint)LayersID::EDITOR] && layers[(uint)LayersID::EDITOR]->IsEnabled()) layers[(uint)LayersID::EDITOR]->PostUpdate();
}

bool ModuleLayers::CleanUp()
{
    XMLNode sceneXML = Application::Instance()->xml->GetConfigXML();

    if (_requestScenePath != "null")
    {
        sceneXML.FindChildBreadth("currentScene").node.attribute("value").set_value(_requestScenePath.c_str());

        ModuleResourceManager::S_SerializeScene(rootGameObject);
    }
    else
    {
        std::string newDir =  ModuleResourceManager::S_GetCurrentWorkingDir();

        newDir += rootGameObject->name + ".HScene";

        sceneXML.FindChildBreadth("currentScene").node.attribute("value").set_value(newDir.c_str());

        ModuleResourceManager::S_SerializeScene(rootGameObject);
    }

    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
       if (layers[i]) layers[i]->CleanUp();
       RELEASE(layers[i]);
    }
    RELEASE(rootGameObject);
    return true;
}

uint ModuleLayers::AddGameObject(GameObject* go, uint ID)
{
    ID = ID == 0 ? HelloUUID::GenerateUUID() : ID;
    gameObjects[ID] = go;
    return ID;
}

void ModuleLayers::RequestLoadScene(const std::string& scenePath)
{
    _requestScenePath = scenePath;

    _requestScene = true;
}
