#pragma once
#include "Layer.h"
#include "MeshRenderComponent.h"
#include "GameObject.h"
#include "API/HelloBehavior.h"
#include <ctime>

#ifdef _DEBUG
#define DLL_DIRECTORY "HelloAPI/bin/Debug/HelloAPI.dll"
#else
#define DLL_DIRECTORY "HelloAPI/bin/Release/HelloAPI.dll"
#endif

#ifdef _DEBUG
#define CHECK_DIRECTORY "HelloAPI/Source/.checkD"
#else
#define CHECK_DIRECTORY "HelloAPI/Source/.checkR"
#endif

class ScriptComponent;

struct BehaviorScript
{
	HelloBehavior* script = nullptr;
	bool active = true;
	bool lateStart = false;
};

class LayerGame : public Layer
{
public:
	LayerGame();
	~LayerGame();

	void Start() override;

	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;
	void CleanUp() override;

	static void S_Play();
	static void S_Stop();
	static void S_Pause();
	static void S_OneFrame();

	static void S_AddScriptComponent(ScriptComponent* component);
	static void S_RemoveScriptComponent(ScriptComponent* component);

	static void S_CreateBehaviorScript(ScriptComponent* component);
	static void S_DestroyBehaviorScript(ScriptComponent* component);

	static bool S_CreateBehaviorScriptByName(const std::string& className, ScriptComponent* component);

	static void S_HotReload();
	static void S_RequestHotReload();
	static void S_RequestDLLCompile();

	static void S_DisableCreatingBehaviors();
	static bool S_IsCreatingBehaviorsEnabled();

	static bool S_IsPlaying() { return _isPlaying; }

	static bool detectInput;

private:
	static std::map<uint, BehaviorScript> _behaviorScripts;
	static std::vector<ScriptComponent*> _scriptComponents;
	static bool _isPlaying;
	static bool _paused;
	static bool _update;
	static bool _oneFrame;
	static bool _needsReload;
	static HMODULE _dllFile;
	static time_t _dllChangeTime;
	static std::string currentScene;
	static bool _centerMouse;
	static bool _canCreateBehaviors; // Boolean that gets put in false when a compilation has to be done in the DLL. For when automatic compilation is off.
	static bool _compileDLL;

	friend class LayerEditor;
	friend class ScriptComponent;
	friend class ImWindowGame;
};

