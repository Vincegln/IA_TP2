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

//This FollowAgent Update function calls the Vehicle Update function and it also
//handles the case of several leaders with the issue of choosing which leader the 
//first FollowAgent must follow in his target list. 
void FollowAgent::Update(double elipsed_time) 
{
	// Vehicle Update function
	Vehicle::Update(elipsed_time);

	// Loop to determine if the FollowAgent must change of current leader or not
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

//This method adds a Vehicle to the target list of a FollowAgent
void FollowAgent::AddTarget(Vehicle * vehicle) 
{
	this->targets.push_back(vehicle);
}

//This method empty the target list of a FollowAgent
void FollowAgent::EmptyTargetList()
{
	targets.clear();
}
