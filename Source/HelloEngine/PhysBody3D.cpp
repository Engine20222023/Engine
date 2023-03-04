#include "Headers.h"
#include "PhysBody3D.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

PhysBody3D::PhysBody3D(btRigidBody* body)
{
	this->body = body;
}

PhysBody3D::~PhysBody3D()
{
	if (body)
		ModulePhysics::S_RemovePhysBody(this);
}

void PhysBody3D::Push(float x, float y, float z)
{
	if (body)
		body->applyCentralImpulse(btVector3(x, y, z));
}

void PhysBody3D::SetVelocity(float x, float y, float z)
{
	if (body)
		body->setLinearVelocity(btVector3(x, y, z));
}

void PhysBody3D::GetTransform(float* matrix) const
{
	if (body && matrix)
		body->getWorldTransform().getOpenGLMatrix(matrix);
}

void PhysBody3D::SetTransform(const float* matrix) const
{
	if (body && matrix)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

void PhysBody3D::SetPos(float x, float y, float z)
{
	if (body) 
	{
		btTransform t = body->getWorldTransform();
		t.setOrigin(btVector3(x, y, z));
		body->setWorldTransform(t);
	}
}

float3 PhysBody3D::GetPos()
{
	btVector3 vec = body->getWorldTransform().getOrigin();

	return float3(vec.x(), vec.y(), vec.z());
}

float3 PhysBody3D::GetVelocity()
{
	btVector3 vec = body->getLinearVelocity();

	return float3(vec.x(), vec.y(), vec.z());
}


void PhysBody3D::Update()
{
	if (isRenderingCol == true) {
		Application::Instance()->renderer3D->renderManager.DrawColliderBox(this);//renderer3D->renderManager.DrawOBB(this);
	}
}

