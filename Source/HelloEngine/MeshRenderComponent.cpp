#include "Headers.h"
#include "MeshRenderComponent.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "RenderManager.h"
#include "InstanceRenderer.h"
#include "Mesh.h"
#include "TextureImporter.h"
#include "ModuleResourceManager.h"
#include "ModuleLayers.h"
#include "LayerEditor.h"
#include "MaterialComponent.h"

MeshRenderComponent::MeshRenderComponent(GameObject* gameObject) : Component(gameObject)
{
	_type = Type::MESH_RENDERER;
	_meshID = -1;
	_instanceID = 0;
	_needsTransformCallback = true;
}

MeshRenderComponent::MeshRenderComponent(GameObject* gameObject, const MeshRenderComponent& copy) : Component(gameObject)
{
	_type = Type::MESH_RENDERER;
	_meshID = -1;
	_instanceID = 0;
	_needsTransformCallback = true;

	if (copy._resource != nullptr)
	{
		CreateMesh(copy._resource->UID, copy.renderType);
	}
}

MeshRenderComponent::~MeshRenderComponent()
{
	if (_meshID != -1)
	{
		DestroyMesh();

		if (_resource != nullptr)
			_resource->Dereference();
		if (_gameObject->HasComponent(Component::Type::MATERIAL))
		{
			_gameObject->GetComponent<MaterialComponent>()->SetMeshRenderer(nullptr);
		}
	}
}

void MeshRenderComponent::CreateMesh(uint resourceUID, MeshRenderType type)
{
	if (_resource != nullptr)
	{
		DestroyMesh();

		_resource->Dereference();
		_resourceUID = _resource->UID;
		_resource = nullptr;
	}

	_resource = (ResourceMesh*)ModuleResourceManager::S_LoadResource(resourceUID);
	
	if (_resource == nullptr)
	{
		_meshID = -1;
		return;
	}

	renderType = type;

	// If we are Instanced, we use 2 IDs, one for our resource and another for our instance identifier.
	if (type == MeshRenderType::INSTANCED)
	{
		_meshID = resourceUID;
		_instanceID = Application::Instance()->renderer3D->renderManager.AddMesh(_resource, MeshRenderType::INSTANCED);
	}
	else // Otherwise, we use only 1, that is the identifier for our mesh.
	{
		_meshID = Application::Instance()->renderer3D->renderManager.AddMesh(_resource, type);
	}

	_vertexNum = _resource->meshInfo.vertices.size();
	_indexNum = _resource->meshInfo.indices.size();
	_gameObject->transform->ForceUpdate();
	GetMesh().component = this;
	GetMesh().isIndependent = renderType != MeshRenderType::INSTANCED;

	if (_gameObject->HasComponent<MaterialComponent>())
	{
		_gameObject->GetComponent<MaterialComponent>()->SetMeshRenderer(this);
		_gameObject->GetComponent<MaterialComponent>()->UpdateMaterial();
	}
}

void MeshRenderComponent::CreateMesh2D()
{
	// Create Instancerenderer2D and have a mehs in it.
	_meshID = Application::Instance()->renderer3D->renderManager.plane2DUID;
	_resource = (ResourceMesh*)ModuleResourceManager::S_LoadResource(_meshID);
	_instanceID = Application::Instance()->renderer3D->renderManager.AddMesh(_resource, MeshRenderType::MESH2D);

}

void MeshRenderComponent::OnTransformCallback(float4x4 worldMatrix)
{
	if (this->_gameObject->IsPendingToDelete())
		return;

	Mesh& mesh = GetMesh();
	mesh.modelMatrix = worldMatrix.Transposed();
	mesh.CalculateBoundingBoxes();
}

void MeshRenderComponent::OnEnable()
{
	GetMesh().draw = true;
}

void MeshRenderComponent::OnDisable()
{
	GetMesh().draw = false;
}

Mesh& MeshRenderComponent::GetMesh()
{
	switch (renderType)
	{
	case MeshRenderType::INSTANCED:
	{
		InstanceRenderer* manager = Application::Instance()->renderer3D->renderManager.GetRenderManager(_meshID);
		Mesh& meshReference = manager->GetMap()[_instanceID];
		return meshReference;
	}
	break;
	case MeshRenderType::INDEPENDENT:
	{
		Mesh& meshReference = Application::Instance()->renderer3D->renderManager._independentMeshes[_meshID];
		return meshReference;
	}
	break;
	case MeshRenderType::TRANSPARENCY:
	{
		Mesh& meshReference = Application::Instance()->renderer3D->renderManager._transparencyMeshes[_meshID];
		return meshReference;
	}
	break;
	}
}

void MeshRenderComponent::DestroyMesh()
{
	if (_meshID == -1) 
		return;

	switch (renderType)
	{
	case MeshRenderType::INSTANCED:
	{
		InstanceRenderer* manager = Application::Instance()->renderer3D->renderManager.GetRenderManager(_meshID, false);
		if (manager != nullptr)
			manager->GetMap().erase(_instanceID);
	}
	break;
	case MeshRenderType::INDEPENDENT:
	{
		Application::Instance()->renderer3D->renderManager._independentMeshes.erase(_meshID);
	}
	break;
	case MeshRenderType::TRANSPARENCY:
	{
		Application::Instance()->renderer3D->renderManager._transparencyMeshes.erase(_meshID);
	}
	break;
	}
	_meshID = -1;
}

std::vector<Vertex>& MeshRenderComponent::GetMeshVertices()
{
	if (_resource != nullptr)
	{
		return _resource->meshInfo.vertices;
	}
}

std::vector<uint>& MeshRenderComponent::GetMeshIndices()
{
	if (_resource != nullptr)
	{
		return _resource->meshInfo.indices;
	}
}

void MeshRenderComponent::ChangeMeshRenderType(MeshRenderType type)
{
	if (renderType == type)
		return;

	if (_resource == nullptr) 
		return;

	DestroyMesh();

	CreateMesh(_resource->UID, type);

	// Update mesh transfrom 
	Mesh& mesh = GetMesh();
	mesh.component = this;
	mesh.modelMatrix = _gameObject->transform->GetGlobalMatrix(true).Transposed(); // Force dirty flag update.
	mesh.CalculateBoundingBoxes();
}

void MeshRenderComponent::SetAs2D()
{
	is2D = true;
	CreateMesh2D();
}

#ifdef STANDALONE
void MeshRenderComponent::OnEditor()
{
	bool created = true;
	if (ImGui::CollapsingHeader("Mesh Renderer", &created, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (_meshID == -1)
		{
			ImGui::TextWrapped("No mesh loaded! Drag an .hmesh file below to load a mesh ");

			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Drag .hmesh here"); ImGui::SameLine();

			// Create Droped mesh
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Mesh"))
				{
					const uint* drop = (uint*)payload->Data;

					CreateMesh(*drop);

					std::string popUpmessage = "Loaded Mesh: ";
					LayerEditor::S_AddPopUpMessage(popUpmessage);

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::HelpMarker("You can find .hmesh files by clicking on any model file (FBX or DAE). They will appear below the file icon in the Project window.");

			if (ImGui::Button("Make2D"))
			{
				SetAs2D();
			}

			return;
		}

		bool auxiliaryBool = _isEnabled;
		if (ImGui::Checkbox("Active##Mesh", &auxiliaryBool))
			auxiliaryBool ? Enable() : Disable();

		Mesh& mesh = GetMesh();

		if (!is2D)
		{
			ImGui::TextWrapped("Mesh vertices: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), std::to_string(_vertexNum).c_str());

			ImGui::TextWrapped("Mesh indices: "); ImGui::SameLine();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), std::to_string(_indexNum).c_str());

			ImGui::Checkbox("Vertex Normals", &mesh.showVertexNormals); ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &mesh.showFaceNormals);

			ImGui::Checkbox("AABB", &mesh.showAABB); ImGui::SameLine();
			ImGui::Checkbox("OBB", &mesh.showOBB);

			currentCombo = (int)renderType;

			if (ImGui::BeginCombo("Render Type", _comboOptions[currentCombo].c_str()))
			{
				for (int i = 0; i < 3; i++)
				{
					bool selected = i == currentCombo;
					if (ImGui::Selectable(_comboOptions[i].c_str(), &selected))
					{
						ChangeMeshRenderType((MeshRenderType)i);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::TextColored(ImVec4(1, 1, 0, 1), _resource->debugName.c_str()); ImGui::SameLine();

			// Create Droped mesh
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Mesh"))
				{
					const uint* drop = (uint*)payload->Data;

					CreateMesh(*drop);

					std::string popUpmessage = "Loaded Mesh: ";
					LayerEditor::S_AddPopUpMessage(popUpmessage);

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::HelpMarker("You can find .hmesh files by clicking on any model file (FBX or DAE). They will appear below the file icon in the Project window.");
		}
		else
		{
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "This is a 2D plane.");
		}
	}
	if (!created)
		this->_gameObject->DestroyComponent(this);
}

void MeshRenderComponent::MarkAsDead()
{
	if (_meshID != -1)
	{
		DestroyMesh();

		if (_resource != nullptr)
		{
			_resource->Dereference();
			_resourceUID = _resource->UID;
			_resource = nullptr;
		}
	}
}

void MeshRenderComponent::MarkAsAlive()
{
	CreateMesh(_resourceUID, renderType);
}

#endif

void MeshRenderComponent::Serialization(json& j)
{
	// We don't want to serialize 2D mesh renderers because their information is saved inside the ComponentUI. We assume every 2D Mesh Renderer is attached to a ComponentUI.
	if (is2D)
		return;

	json _j;

	_j["Type"] = _type;

	if (_meshID != -1)
	{
		Mesh& m = GetMesh();
		_j["ShowAABB"] = m.showAABB;
		_j["ShowOBB"] = m.showOBB;
		_j["ShowFaceNormals"] = m.showFaceNormals;
		_j["ShowVertexNormals"] = m.showVertexNormals;
	}
	else
	{
		_j["ShowAABB"] = false;
		_j["ShowOBB"] = false;
		_j["ShowFaceNormals"] = false;
		_j["ShowVertexNormals"] = false;
	}

	if (_resource != nullptr)
	{
		_j["ModelUID"] = _resource->modelUID;
		_j["Index inside model"] = _resource->indexInsideModel;
	}
	else
	{
		_j["ModelUID"] = 0;
		_j["Index inside model"] = 0;
	}

	_j["Enabled"] = _isEnabled;

	j["Components"].push_back(_j);
}

void MeshRenderComponent::DeSerialization(json& j)
{
	ResourceModel* model = (ResourceModel*)ModuleResourceManager::resources[j["ModelUID"]];

	if (model == nullptr)
	{
		Console::S_Log("A scene mesh render data was not found.");
		return;
	}
	
	uint index = j["Index inside model"];
	if (index < model->modelMeshes.size())
	{
		ResourceMesh* resourceMesh = model->modelMeshes[index];
		if (resourceMesh->UID == Application::Instance()->renderer3D->renderManager.plane2DUID)
		{
			SetAs2D();
		}
		else
		{
			CreateMesh(resourceMesh->UID);

			if (_meshID != -1)
			{
				Mesh& m = GetMesh();

				m.showAABB = j["ShowAABB"];
				m.showOBB = j["ShowOBB"];
				m.showFaceNormals = j["ShowFaceNormals"];
				m.showVertexNormals = j["ShowVertexNormals"];
			}
		}
	}
	
	bool enabled = j["Enabled"];
	if (!enabled)
		Disable();
		
}

uint MeshRenderComponent::GetResourceUID()
{
	if (this->_resource != nullptr)
		return _resource->UID;
	return 0;
}

void MeshRenderComponent::DestroyedResource()
{
	CreateMesh(0);
}

void MeshRenderComponent::UnlinkResource()
{
	if (_resource != nullptr)
	{
		_resource->Dereference();
		_resourceUID = _resource->UID;
		_resource = nullptr;
	}
}
