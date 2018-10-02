#pragma once

#include "Vehicle.h"
#include "SteeringBehaviors.h"
#include "GameWorld.h"

class GameWorld;
class SteeringBehavior;

class LeaderAgent : public Vehicle
{

private:

	// Flag to know is the leader agent is moving
	bool moving;
public:

	// Constructor
	LeaderAgent(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale);

	// Destructor
	~LeaderAgent();

	//-------------------------------------------accessor methods

	void OnMoving() { this->Steering()->WanderOn(); moving = true; };
	void OffMoving() { this->Steering()->WanderOff(); moving = false; };


	
};

