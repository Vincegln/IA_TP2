#include "FollowAgent.h"
#include "SteeringBehaviors.h"

// Constructor
FollowAgent::FollowAgent(GameWorld* world,
	Vector2D position,
	double    rotation,
	Vector2D velocity,
	double    mass,
	double    max_force,
	double    max_speed,
	double    max_turn_rate,
	double    scale) 
	:
	Vehicle(world,
		position,
		rotation,
		velocity,
		mass,
		max_force,
		max_speed,
		max_turn_rate,
		scale)
{
	// By default, a follow agent doesn't have a steeringBehavior
	flocking = false;
	pursuit = false;
}

// Destructor
FollowAgent::~FollowAgent()
{
	Vehicle::~Vehicle();
}


