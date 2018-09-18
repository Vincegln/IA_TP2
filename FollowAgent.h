#pragma once
#include "Vehicle.h"

class GameWorld;
class SteeringBehavior;

class FollowAgent : public Vehicle
{

private:
	bool separation;
	bool alignment;
	bool cohesion;
	bool pursuit;

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


	//-------------------------------------------accessor methods

	void OnSeperation() { this->Steering()->SeparationOn(); separation = true; };
	void OffSeperation() { this->Steering()->SeparationOff(); separation = false; };
	
	void OnAlignment() { this->Steering()->AlignmentOn(); alignment = true; };
	void OffAlignment() { this->Steering()->AlignmentOff(); alignment = false; };

	void OnCohesion() { this->Steering()->CohesionOn(); cohesion = true; };
	void OffCohesion() { this->Steering()->CohesionOff(); cohesion = false; };

	void OnPursuit() { this->Steering()->OffsetPursuitOn(this, Vector2D(1, 0)); pursuit = true; };
	void OffPursuit() { this->Steering()->OffsetPursuitOff(); pursuit = false; };


};

