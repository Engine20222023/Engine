#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"
//#include "Timer.hpp"
//#include <Timer.hpp>
#include "API/API.h"

class EnemyMeleeMovement : HelloBehavior
{
public:
	enum class States {
		TARGETING,
		WANDERING,
		ATTACKIG

	};
	void Start() override; 
	void Update() override;

	void Seek(float vel, API_Vector3 tarPos);
	void Wander(float vel, API_Vector3 point);

	API_Vector3 NormalizeVec3(float x, float y, float z);

	float Lerp(float a, float b, float time);

	bool wander = false;
	bool targeting = false;

	float detectionDis=2.0f;
	float lossingDis = 10.0f;
	float lossingZoneDis = 200.0f;

	//API_GameObject point1;
	//API_GameObject point2;

	API_GameObject target; 
	API_GameObject actionZone; 

	API_RigidBody zoneRb;
	
	API_GameObject listPoints[5];
	int numPoint = 0;
	
	API_Vector3 actualPoint;

	States enemState;


private:
	int _avalPoints = 0;
	//Timer clock;
};




