#pragma once
#include "Module.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class Primitive;
class PhysBody3D;

enum class ColliderShape
{
	BOX,
	SPHERE,
	CYLINDER,
	Count,
	NONE,

};

class ModulePhysics : public Module
{
public:

	ModulePhysics();

	virtual ~ModulePhysics();

	bool Init() override;

	bool Start() override;

	UpdateStatus PreUpdate() override;

	UpdateStatus Update() override;

	UpdateStatus PostUpdate() override;

	bool CleanUp() override;

	PhysBody3D* CreatePhysBody(const Primitive* primitive, float mass = 1.0f);
	void RemovePhysBody(PhysBody3D* physBody);
	//PhysBody3D* CreatePhysBody();
	//PhysBody3D* CreatePhysBody();

private:

	btDefaultCollisionConfiguration* collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	bool debugDraw = false;

	PhysBody3D* testBody = nullptr;

	std::vector <btCollisionShape*> shapes;
	std::vector <PhysBody3D*> bodies;
	std::vector <btDefaultMotionState*> motions;
	std::vector <btTypedConstraint*> constraints;
};