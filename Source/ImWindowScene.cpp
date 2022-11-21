#include "Headers.h"
#include "ImWindowScene.h"
#include "ModuleCamera3D.h"
#include "ModuleResourceManager.h"
#include "ModuleLayers.h"
#include "LayerEditor.h"

ImWindowScene::ImWindowScene()
{
	windowName = "Scene";

	isEnabled = true;

	moduleCamera = Application::Instance()->camera;
	sceneCamera = &Application::Instance()->camera->sceneCamera;
}

ImWindowScene::~ImWindowScene()
{
}

void ImWindowScene::Update()
{
	if (ImGui::Begin(windowName.c_str()))
	{
		ImGui::BeginChild("DropArea");
		{
			sceneCamera->active = true;

			moduleCamera->updateSceneCamera = ImGui::IsWindowHovered();

			ImVec2 sceneDimensions = ImGui::GetContentRegionAvail();

			if (sceneDimensions.x != sceneWidth || sceneDimensions.y != sceneHeight)
			{
				// If the size of this imgui window is different from the one stored.
				sceneWidth = sceneDimensions.x;
				sceneHeight = sceneDimensions.y;
				sceneCamera->ChangeAspectRatio((float)sceneWidth / (float)sceneHeight);
			}
			//ImGuizmo::DrawGrid(Application::Instance()->camera->sceneCamera.GetViewMatrix(), Application::Instance()->camera->sceneCamera.GetProjectionMatrix(), &identity.v[0][0], 100);

			DetectSceneInput();

			ImGui::Image((ImTextureID)sceneCamera->frameBuffer.GetTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

			GameObject* selected = Application::Instance()->layers->editor->selectedGameObject;

			if (selected != nullptr)
			{
				float4x4 auxiliarMatrix = selected->transform->GetGlobalMatrix();
				auxiliarMatrix.Transpose();

				// Could be done only when one of the 4 variables changes.
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
				ImGuizmo::SetDrawlist();
				if (ImGuizmo::Manipulate(sceneCamera->GetViewMatrix(), sceneCamera->GetProjectionMatrix(), _imOperation, ImGuizmo::MODE::LOCAL, &auxiliarMatrix.v[0][0]))
				{
					auxiliarMatrix.Transpose();
					selected->transform->SetTransform(auxiliarMatrix);
				}

			}
		}
		ImGui::EndChild();

		// Create Droped mesh
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Mesh"))
			{
				const std::string drop = *(std::string*)payload->Data;

				Application::Instance()->resource->LoadFile(drop);

				std::string popUpmessage = "Loaded Mesh: " + drop;
				Application::Instance()->layers->editor->AddPopUpMessage(popUpmessage);

			}
			ImGui::EndDragDropTarget();
		}
	}
	else
	{
		sceneCamera->active = false;
	}
	
	ImGui::End();
}

void ImWindowScene::DetectSceneInput()
{
	if (Application::Instance()->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		_imOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	else if (Application::Instance()->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		_imOperation = ImGuizmo::OPERATION::ROTATE;
	}
	else if (Application::Instance()->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		_imOperation = ImGuizmo::OPERATION::SCALE;
	}
}
