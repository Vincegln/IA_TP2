#pragma once
#include "Vehicle.h"
#include "SteeringBehaviors.h"
#include "GameWorld.h"
#include <vector>

using namespace std;

class GameWorld;
class SteeringBehavior;

class FollowAgent : public Vehicle
{

private:
	// flags who allow us to know if these steeringBehaviors are activated

	// Flag to know if the flocking is enable
	bool flocking;

	// Flag to know if the pursuit is enable
	bool pursuit;

	// List of the targets that the FollowAgent will choose to follow
	// by the distance between its targets and itself
	vector<Vehicle*> targets;
public:

	// Constructor
	FollowAgent(GameWorld* world,
		Vector2D position,
		double    rotation,
		Vector2D velocity,
		double    mass,
		double    max_force,
		double    max_speed,
		double    max_turn_rate,
		double    scale);

	// Destructor
	~FollowAgent();


	//-------------------------------------------accessor methods
	void FlockingOn() { this->Steering()->FlockingOn(); flocking = true; }
	void FlockingOff() { this->Steering()->FlockingOff(); flocking = false; };

	void OnPursuit(Vehicle* vehicle, Vector2D offset) { this->Steering()->OffsetPursuitOn(vehicle, offset); pursuit = true; };
	void OffPursuit() { this->Steering()->OffsetPursuitOff(); pursuit = false; };

	void Update(double time_elapsed);

	void AddTarget(Vehicle * vehicle);
	void EmptyTargetList();
};

