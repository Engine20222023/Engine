#include "GetDiviner.h"
#include "../../Player/PlayerStorage.h"
HELLO_ENGINE_API_C GetDiviner* CreateGetDiviner(ScriptToInspectorInterface* script)
{
    GetDiviner* classInstance = new GetDiviner();
    //Show variables inside the inspector using script->AddDragInt("variableName", &classInstance->variable);
    script->AddDragBoxGameObject("Player Storage GO", &classInstance->playerStorageGO);
    script->AddDragBoxGameObject("Final Text Panel", &classInstance->finalText);
    return classInstance;
}

void GetDiviner::Start()
{
    playerStorage = (PlayerStorage*)playerStorageGO.GetScript("PlayerStorage");
    if (playerStorage == nullptr) Console::Log("PlayerStorage missing in GetDiviner Script.");
}

void GetDiviner::Update()
{

}

void GetDiviner::OnCollisionEnter(API_RigidBody other)
{
    std::string detectionName = other.GetGameObject().GetName();
    if (detectionName == "Player")
    {
        if (Input::GetGamePadButton(GamePadButton::BUTTON_X) == KeyState::KEY_DOWN || Input::GetKey(KeyCode::KEY_E) == KeyState::KEY_DOWN)
        {
            if (playerStorage)
            {
                playerStorage->skillPoints += 5;
                playerStorage->SaveData();
            }
            API_QuickSave::SetBool("level1_completed", true);
            finalText.SetActive(true);
        }
        
    }
}