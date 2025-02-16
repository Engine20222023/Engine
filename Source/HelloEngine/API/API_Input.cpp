#include "Headers.h"
#include "API_Input.h"
#include "ModuleInput.h"
#include "LayerGame.h"

KeyState API::Input::GetKey(KeyCode key)
{
	if (!LayerGame::detectInput)
		return KeyState::KEY_IDLE;
	return (KeyState)ModuleInput::S_GetKey((int)key);
}

KeyState API::Input::GetMouseButton(MouseButton mouseButton)
{
	if (!LayerGame::detectInput)
		return KeyState::KEY_IDLE;
	return (KeyState)ModuleInput::S_GetMouseButton((int)mouseButton);
}

int API::Input::GetMouseX()
{
	if (!LayerGame::detectInput)
		return 0;
	return ModuleInput::S_GetMouseX();
}

int API::Input::GetMouseY()
{
	if (!LayerGame::detectInput)
		return 0;
	return ModuleInput::S_GetMouseY();
}

int API::Input::GetMouseZ()
{
	if (!LayerGame::detectInput)
		return 0;
	return ModuleInput::S_GetMouseZ();
}

int API::Input::GetMouseXMotion()
{
	if (!LayerGame::detectInput)
		return 0;
	return ModuleInput::S_GetMouseXMotion();
}

int API::Input::GetMouseYMotion()
{
	if (!LayerGame::detectInput)
		return 0;
	return ModuleInput::S_GetMouseYMotion();
}
