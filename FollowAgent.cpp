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

void FollowAgent::Update(double elipsed_time) 
{
	Vehicle::Update(elipsed_time);
	if (targets.size() > 1) 
	{
		int currentIndex = 0;
		int indexLeader = 0;
		double distanceMin = 100000000000000;
		for (vector<Vehicle *>::iterator it = targets.begin(); it != targets.end(); it++) 
		{
			if ((this->Pos().Distance((*it)->Pos())) < distanceMin) 
			{
				indexLeader = currentIndex;
				distanceMin = (this->Pos().Distance((*it)->Pos()));
			}
			currentIndex++;
		}
		this->Steering()->SetTargetAgent1(targets.at(indexLeader));
	}
}

void FollowAgent::AddTarget(Vehicle * vehicle) 
{
	this->targets.push_back(vehicle);
}

void FollowAgent::EmptyTargetList()
{
	targets.clear();
}
