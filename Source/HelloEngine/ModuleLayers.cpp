#include "Headers.h"
#include "ModuleLayers.h"
#include "LayerEditor.h"
#include "LayerGame.h"
#include "LayerUI.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "TransformComponent.h"
#include "ModuleResourceManager.h"

#include "ModuleInput.h"
#include "ModuleWindow.h"

//TEMPORAL
#include "ComponentUI.h"

LayerGame* ModuleLayers::game = nullptr;
LayerUI* ModuleLayers::layerUI = nullptr;
GameObject* ModuleLayers::rootGameObject = nullptr;
LayerEditor* ModuleLayers::editor = nullptr;
std::map<uint, GameObject*> ModuleLayers::gameObjects;
Layer* ModuleLayers::_layers[(uint)LayersID::MAX] = { nullptr };
bool ModuleLayers::_requestScene = false;
std::string ModuleLayers:: _requestScenePath = "null";
std::string ModuleLayers::_sceneBeginPath = "null";
std::vector<GameObject*> ModuleLayers::_deletedGameObjects;
std::vector<API::API_GameObject*> ModuleLayers::apiGameObjects;

API::API_Transform* ModuleLayers::emptyAPITransform = nullptr;
API::API_GameObject* ModuleLayers::emptyAPIGameObject = nullptr;

ModuleLayers::ModuleLayers()
{
}

ModuleLayers::~ModuleLayers()
{
}

bool ModuleLayers::Start()
{
    // Create empty API components.
    emptyAPITransform = new API::API_Transform();

#ifdef STANDALONE
    _layers[(uint)LayersID::EDITOR] = new LayerEditor();
#endif
    _layers[(uint)LayersID::GAME] = new LayerGame();
    _layers[(uint)LayersID::UI] = new LayerUI();

    game = (LayerGame*)_layers[(uint)LayersID::GAME];
    layerUI = (LayerUI*)_layers[(uint)LayersID::UI];
    editor = (LayerEditor*)_layers[(uint)LayersID::EDITOR];

    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
        if (_layers[i] && _layers[i]->IsEnabled())
            _layers[i]->Start();
    }

    // Create Root GameObject (Scene)
    XMLNode sceneXML = Application::Instance()->xml->GetConfigXML();

    _sceneBeginPath = sceneXML.FindChildBreadth("currentScene").node.attribute("value").as_string();

    if (!ModuleResourceManager::S_DeserializeScene(_sceneBeginPath))
    {
        rootGameObject = new GameObject(nullptr, "Root", "None");
        XMLNode sceneXML = Application::Instance()->xml->GetConfigXML();

        std::string newDir = ASSETS_PATH;

        // Change Title
        newDir += rootGameObject->name + ".HScene";

        std::string scenePath = " -- CurrentScene: " + newDir;

        ModuleWindow::S_AddTitleExtraInfo(scenePath);

        sceneXML.FindChildBreadth("currentScene").node.attribute("value").set_value(newDir.c_str());
        
        ModuleResourceManager::S_SerializeScene(rootGameObject);
    }

    Console::S_Log("Error message test", LogType::ERR);
    Console::S_Log("Warning message test", LogType::WARNING);


    return true;
}

UpdateStatus ModuleLayers::PreUpdate()
{
    if(_requestScene)
    {
        if(!ModuleResourceManager::S_DeserializeScene(_requestScenePath))
            _requestScenePath = "null";

        _requestScene = false;
    }

    for (int i = 0; i < _deletedGameObjects.size(); i++)
    {
        RELEASE(_deletedGameObjects[i]);
    }
    _deletedGameObjects.clear();

    for (int i = (uint)LayersID::MAX; i > 0; i--)
    {
        if (_layers[i] && _layers[i]->IsEnabled()) 
            _layers[i]->PreUpdate();
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleLayers::Update()
{
    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
        if (_layers[i] && _layers[i]->IsEnabled()) 
            _layers[i]->Update();
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleLayers::PostUpdate()
{
    return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleLayers::S_DrawLayers()
{
    for (int i = 0; i < (uint)LayersID::EDITOR; i++)
    {
        if (_layers[i] && _layers[i]->IsEnabled())
            _layers[i]->PostUpdate();
    }
}

void ModuleLayers::S_DrawEditor()
{
    if (_layers[(uint)LayersID::EDITOR] && _layers[(uint)LayersID::EDITOR]->IsEnabled()) 
        _layers[(uint)LayersID::EDITOR]->PostUpdate();
}

bool ModuleLayers::CleanUp()
{
    XMLNode sceneXML = Application::Instance()->xml->GetConfigXML();

    std::string configScene = sceneXML.FindChildBreadth("currentScene").node.attribute("value").as_string();

    if (configScene == "null")
    {
        std::string newDir = ASSETS_PATH;

        newDir += rootGameObject->name + ".HScene";

        sceneXML.FindChildBreadth("currentScene").node.attribute("value").set_value(newDir.c_str());
    }

    if (!LayerGame::S_IsPlaying())
        ModuleResourceManager::S_SerializeScene(rootGameObject);

    for (int i = 0; i < (uint)LayersID::MAX; i++)
    {
       if (_layers[i]) 
           _layers[i]->CleanUp();
       
       RELEASE(_layers[i]);
    }
   
    RELEASE(rootGameObject);
    RELEASE(emptyAPITransform);
    return true;
}

uint ModuleLayers::S_AddGameObject(GameObject* go, uint ID)
{
    ID = ID == 0 ? HelloUUID::GenerateUUID() : ID;
    gameObjects[ID] = go;
    return ID;
}

void ModuleLayers::S_RemoveGameObject(uint ID)
{
    GameObject* goPointer = gameObjects[ID];
    gameObjects.erase(ID);

    // Look for the deleted game object inside the API_GameObject vector.
    for (int i = 0; i < apiGameObjects.size(); ++i)
    {
        if (apiGameObjects[i]->_gameObject == goPointer)
        {
            apiGameObjects[i]->_gameObject = nullptr;
        }
    }
}

void ModuleLayers::S_RequestLoadScene(const std::string& scenePath)
{
    _requestScenePath = scenePath;

    _requestScene = true;
}

GameObject* ModuleLayers::S_GetGameObject(uint ID)
{
    if (gameObjects.find(ID) != gameObjects.end())
    {
        return gameObjects[ID];
    }
    return nullptr;
}

void ModuleLayers::DestroyMeshes()
{
    for (auto& gameObject : gameObjects)
    {
        MeshRenderComponent* mesh = gameObject.second->GetComponent<MeshRenderComponent>();
        if (mesh != nullptr)
            mesh->DestroyMesh();  
    }
}
