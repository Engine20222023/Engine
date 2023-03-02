#pragma once
#include "Component.h"
#include "Globals.h"
#include "PhysBody3D.h"
#include "ModulePhysics.h"

class PhysicsComponent : public Component
{
public:

	PhysicsComponent(GameObject* gameObject);

private:

	~PhysicsComponent();
	
	void OnEditor() override;

	void Serialization(json& j) override;
	void DeSerialization(json& j) override;

	//Physics
	void CheckShapes();

	void CallUpdateShape();

	void CreateCollider();

private:

	PhysBody3D* physBody;

	ColliderShape shapeSelected;

	bool isShapeSelected[3];
	bool isShapeCreated[3];
	bool isStatic;

	float3 colPos;
	float3 colRot;
	float3 colScl;

	float sphereRadius;
	float2 cylRadiusHeight;

};


