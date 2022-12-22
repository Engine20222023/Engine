#include "Headers.h"
#include "LayerGame.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "MeshImporter.h"
#include "TextureImporter.h"

LayerGame::LayerGame()
{
	
}

LayerGame::~LayerGame()
{
}

void LayerGame::Start()
{
}

void LayerGame::PreUpdate()
{
	if ((!_isPlaying || _paused) && !_oneFrame)
	{
		_update = false;
		return;
	}
	_oneFrame = false;
	_update = true;
	
	// Update time.
	Time::UpdateRealTime();
	Time::UpdateGameTime();

	for (auto& script : _scripts)
	{
		script.second->Update();
	}
}

void LayerGame::Update()
{
	if (!_update)
		return;
}

void LayerGame::PostUpdate()
{
	if (!_update)
		return;
}

void LayerGame::Play()
{
	// TODO: Save scene.
	Time::Reset();
	Time::Start();
	_isPlaying = true;

	for (auto& script : _scripts)
	{
		script.second->Start();
	}

	/*_paused = false;*/
}

void LayerGame::Stop()
{
	Time::Reset();
	_isPlaying = false;
	_paused = false;
	// TODO: Reload scene.
}

void LayerGame::Pause()
{
	//if (!_isPlaying)
	//	return;

	_paused = !_paused;
	if (!_paused)
		Time::Start();
}

void LayerGame::OneFrame()
{
	if (!_isPlaying || !_paused) return;
	Time::Start();
	_oneFrame = true;
}

void LayerGame::AddScript(uint UID, HelloBehavior* script)
{
	_scripts[UID] = script;
}

void LayerGame::DestroyScripts()
{
	for (auto& script : _scripts)
	{
		RELEASE(script.second);
	}
}

void LayerGame::CleanUp()
{
}
