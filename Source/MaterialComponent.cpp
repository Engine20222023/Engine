#include "Headers.h"
#include "MaterialComponent.h"
#include "RenderManager.h"
#include "GameObject.h"
#include "TextureImporter.h"
#include "ModuleResourceManager.h"
#include "TextureManager.h"

MaterialComponent::MaterialComponent(GameObject* go) : Component(go)
{
	MeshRenderComponent* meshRenderer = go->GetComponent<MeshRenderComponent>();
	if (!meshRenderer) return;
	SetMeshRenderer(meshRenderer);
}

MaterialComponent::~MaterialComponent()
{
}

Mesh& MaterialComponent::GetMesh()
{
	return meshRenderer->GetMesh();
}

void MaterialComponent::OnEditor()
{
	if (!ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) return;

	if (!meshRenderer)
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "No MeshRenderComponent detected!");

		if (ImGui::Button("Search MeshRenderComponent"))
		{
			MeshRenderComponent* meshRenderer = _gameObject->GetComponent<MeshRenderComponent>();
			if (!meshRenderer) return;
			SetMeshRenderer(meshRenderer);
		}
	}
	else
	{
		Mesh& mesh = GetMesh();

		if (ImGui::Button("Set Checkers Texture"))
		{
			mesh.textureID = TextureImporter::CheckerImage();
		}

		std::string imageName;
		int width = 0;
		int height = 0;
		if (mesh.textureID != -1.0f)
		{
			ImGui::Image((ImTextureID)(uint)mesh.textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			Texture text = TextureManager::loadedTextures[mesh.textureID];
			imageName = text.name;
			width = text.width;
			height = text.height;
		}
		else 
		{
			ImGui::Image((ImTextureID)0, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			imageName = "None";
		}
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
			{
				//Drop asset from Asset window to scene window
				const std::string drop = *(std::string*)payload->Data;

				Resource* resource = Application::Instance()->resource->LoadFile(drop);

				if (resource->type != ResourceType::TEXTURE) return;

				ResourceTexture* textureResource = (ResourceTexture*)resource;

				mesh.textureID = textureResource->textureInfo.OpenGLID;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::TextWrapped("Path: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), imageName.c_str());

		ImGui::TextWrapped("Width: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), std::to_string(width).c_str());

		ImGui::TextWrapped("Height: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), std::to_string(height).c_str());
	}
}

void MaterialComponent::SetMeshRenderer(MeshRenderComponent* mesh)
{
	this->meshRenderer = mesh;
}

void MaterialComponent::ChangeTexture(uint textureID)
{
	GetMesh();
}
