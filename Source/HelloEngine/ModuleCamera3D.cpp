#include "Headers.h"
#include <iostream>
#include "Application.h"
#include "ModuleCamera3D.h"

#include "ModuleWindow.h"
#include "GameObject.h"
#include "ModuleLayers.h"
#include "CameraComponent.h"
#include "SceneCameraObject.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(sceneCamera);
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
#ifdef STANDALONE
	sceneCamera = new SceneCameraObject(); // Needs to be allocated manually to avoid initializtion order issues.
	sceneCamera->frameBuffer.SetBufferInfo();
	sceneCamera->frameBuffer.SetDimensions(ModuleWindow::width, ModuleWindow::height);
	sceneCamera->isCullingActive = false;
	sceneCamera->cameraFrustum.farPlaneDistance = 4000;
#endif // STANDALONE

	
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::DrawCameraFrustums()
{
	for (int i = 0; i < gameCameras.size(); i++)
	{
		gameCameras[i]->DrawFrustum();
	}
}

void ModuleCamera3D::RequestFrameBufferRegen(CameraObject* camera, int width, int height)
{
	_frameBufferRegenCamera = camera;
	_newBufferWidth = width;
	_newBufferHeight = height;
}

CameraObject* ModuleCamera3D::CreateGameCamera()
{
	CameraObject* newCamera = new CameraObject();
	gameCameras.push_back(newCamera);
	if (gameCameras.size() == 1)
	{
		activeGameCamera = newCamera; // If this is the first game camera, assign the current display camera to this.
		newCamera->currentlyDisplaying = true;
	}
	return newCamera;
}

void ModuleCamera3D::SetCurrentActiveGameCamera(CameraObject* target)
{
	if (activeGameCamera != nullptr)
		activeGameCamera->currentlyDisplaying = false;

	activeGameCamera = target;

	target->currentlyDisplaying = true;
}

void ModuleCamera3D::EraseGameCamera(CameraObject* erasedCamera)
{
	// TODO: To be tested when deleting game objects is possible.
	for (int i = 0; i < gameCameras.size(); i++)
	{
		if (erasedCamera == gameCameras[i]) 
		{
			gameCameras.erase(gameCameras.begin() + i);
			if (activeGameCamera == erasedCamera)
			{
				activeGameCamera = gameCameras.empty() ? nullptr : gameCameras[0];

				if (activeGameCamera != nullptr)
					activeGameCamera->currentlyDisplaying = true;
			}
			break;
		}
	}
	
}

// -----------------------------------------------------------------
UpdateStatus ModuleCamera3D::Update()
{
	if (_frameBufferRegenCamera != nullptr)
	{
		_frameBufferRegenCamera->RegenerateFrameBuffer(_newBufferWidth, _newBufferHeight);
		_frameBufferRegenCamera = nullptr;
	}
#ifdef STANDALONE
	if (updateSceneCamera) sceneCamera->UpdateInput();
#endif // STANDALONE


	return UpdateStatus::UPDATE_CONTINUE;
}

