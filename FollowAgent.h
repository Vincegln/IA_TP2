#pragma once
#include "Vehicle.h"

class GameWorld;
class SteeringBehavior;

class FollowAgent : public Vehicle
{
public:

	FollowAgent(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale);

	~FollowAgent();

};

