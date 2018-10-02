#include "LeaderAgent.h"
#include "SteeringBehaviors.h"


// Constructor
LeaderAgent::LeaderAgent(GameWorld * world, Vector2D position, double rotation, Vector2D velocity, double mass, double max_force, double max_speed, double max_turn_rate, double scale) :Vehicle(world,
	position,
	rotation,
	velocity,
	mass,
	max_force,
	max_speed,
	max_turn_rate,
	scale)
{
	// By default, a leader agent doesn't move
	moving = false;
}

// Destructor
LeaderAgent::~LeaderAgent()
{
	Vehicle::~Vehicle();
}
