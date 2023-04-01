#include "Headers.h"
#include "ModuleLayers.h"
#include "API_UIText.h"
#include "TextRendererComponent.h"

API::API_UIText::API_UIText()
{
}

API::API_UIText::~API_UIText()
{
}

API::API_GameObject API::API_UIText::GetGameObject()
{
	if (!_UIText)
	{
		Engine::Console::S_Log("Trying to acces a NULLPTR UIText");
		return *ModuleLayers::emptyAPIGameObject;
	}
	API_GameObject returnGO;
	returnGO.SetGameObject(_UIText->GetGameObject());
	return returnGO;
}


ComponentUIText* API::API_UIText::GetComponent()
{
	return _UIText;
}

void API::API_UIText::SetComponent(ComponentUIText* component)
{
	_UIText = component;
}


