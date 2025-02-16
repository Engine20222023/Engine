#pragma once
#include "Globals.h"
#include <string>
#include <vector>
#include "API/API_GameObject.h"

#ifndef HELLO_ENGINE_EXPORTS
#include "json.hpp"
using json = nlohmann::json;
#endif

class ScriptInspectorField
{
public:
	ScriptInspectorField();
	void* value = nullptr;
	std::string valueName = "";
	std::string className = "";
	uint UID = 0;
	virtual void OnEditor() = 0;
#ifndef HELLO_ENGINE_EXPORTS
	virtual void OnSerialize(json& j) = 0;
	virtual void OnDeserialize(json& j) = 0;
#endif
};

class DragFieldFloat : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class DragFieldInt : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class CheckBoxField : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class InputBoxField : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class DragBoxGameObject : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class DragBoxTransform : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class DragBoxMeshRenderer : public ScriptInspectorField
{
	void OnEditor() override;
#ifndef HELLO_ENGINE_EXPORTS
	void OnSerialize(json& j) override;
	void OnDeserialize(json& j) override;
#endif
};

class TO_API ScriptToInspectorInterface
{
public:
	virtual void AddDragFloat(const char* name, float* value) = 0;
	virtual void AddDragInt(const char* name, int* value) = 0;
	virtual void AddCheckBox(const char* name, bool* value) = 0;
	virtual void AddInputBox(const char* name, std::string* value) = 0;
	virtual void AddDragBoxGameObject(const char* name, API::API_GameObject* value) = 0;
	virtual void AddDragBoxTransform(const char* name, API::API_Transform* value) = 0;
	virtual void AddDragBoxMeshRenderer(const char* name, API::API_MeshRenderer* value) = 0;

protected:
	std::vector<ScriptInspectorField*> inspectorFields;
};
