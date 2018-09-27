#include "GameWorld.h"
#include "Vehicle.h"
#include "FollowAgent.h"
#include "LeaderAgent.h"
#include "HumanLeader.h"
#include "constants.h"
#include "Obstacle.h"
#include "2d/Geometry.h"
#include "2d/Wall2D.h"
#include "2d/Transformations.h"
#include "SteeringBehaviors.h"
#include "time/PrecisionTimer.h"
#include "misc/Smoother.h"
#include "ParamLoader.h"
#include "misc/WindowUtils.h"
#include "misc/Stream_Utility_Functions.h"
#include <iostream>
#include "resource.h"

#include <list>
using std::list;
using namespace std;

// Creation of the two LeaderAgent and HumanAgent
Vehicle* pLeader_temp;
Vehicle* pSecondLeader_temp;
Vehicle* pHumanLeader_temp;
Vehicle* pFollow_Temp;



//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
GameWorld::GameWorld(int cx, int cy):

            m_cxClient(cx),
            m_cyClient(cy),
            m_bPaused(false),
            m_vCrosshair(Vector2D(cxClient()/2.0, cxClient()/2.0)),
            m_bShowWalls(false),
            m_bShowObstacles(false),
            m_bShowPath(false),
            m_bShowWanderCircle(false),
            m_bShowSteeringForce(false),
            m_bShowFeelers(false),
            m_bShowDetectionBox(false),
            m_bShowFPS(true),
            m_dAvFrameTime(0),
            m_pPath(NULL),
            m_bRenderNeighbors(false),
            m_bViewKeys(false),
            m_bShowCellSpaceInfo(false),
			m_bHumanLeader(false),
			m_bOneLeader(true),
			m_bTwoLeader(false),
			m_bTwoOffset(false),
			m_bFiveOffset(true),
			m_bTenOffset(false),
			m_bTwentyAgent(true),
			m_bFiftyAgent(false),
			m_bHundredAgent(false),
			m_bWeightedSum(false)
{

  //setup the spatial subdivision class
  m_pCellSpace = new CellSpacePartition<Vehicle*>((double)cx, (double)cy, Prm.NumCellsX, Prm.NumCellsY, Prm.NumAgents);

  double border = 30;
  m_pPath = new Path(5, border, border, cx-border, cy-border, true); 

  //determine a random starting position
  Vector2D SpawnPos = Vector2D(cx / 2.0 + RandomClamped()*cx / 2.0,
	  cy / 2.0 + RandomClamped()*cy / 2.0);

  // Creation of the LeaderAgent
  Vehicle* pLeader = new LeaderAgent(this,
	  SpawnPos,                 //initial position
	  RandFloat()*TwoPi,        //start rotation
	  Vector2D(100, 100),            //velocity
	  Prm.VehicleMass,          //mass
	  Prm.MaxSteeringForce,     //max force
	  Prm.MaxSpeed,             //max velocity
	  Prm.MaxTurnRatePerSecond, //max turn rate
	  Prm.VehicleScale);        //scale

  pLeader_temp = pLeader;

  dynamic_cast<LeaderAgent*>(pLeader)->OnMoving();
  m_Vehicles.push_back(pLeader);

  // Creation of the second LeaderAgent
  Vehicle* pSecondLeader = new LeaderAgent(this,
	  SpawnPos,                 //initial position
	  RandFloat()*TwoPi,        //start rotation
	  Vector2D(0, 0),            //velocity
	  Prm.VehicleMass,          //mass
	  Prm.MaxSteeringForce,     //max force
	  Prm.MaxSpeed,             //max velocity
	  Prm.MaxTurnRatePerSecond, //max turn rate
	  Prm.VehicleScale);        //scale

  pSecondLeader_temp = pSecondLeader;

  dynamic_cast<LeaderAgent*>(pLeader)->OnMoving();

  // Creation of the HumanLeader

  Vehicle* pHumanLeader = new HumanLeader(this,
	  SpawnPos,                 //initial position
	  RandFloat()*TwoPi,        //start rotation
	  Vector2D(0, 0),            //velocity
	  Prm.VehicleMass,          //mass
	  Prm.MaxSteeringForce,     //max force
	  Prm.MaxSpeed,             //max velocity
	  Prm.MaxTurnRatePerSecond, //max turn rate
	  Prm.VehicleScale);        //scale

  pHumanLeader_temp = pHumanLeader;
  

  Vehicle* pVehicleTemp = pLeader;

  //setup the agents
  for (int a=0; a<Prm.NumAgents; ++a)
  {
	  //determine a random starting position
	  Vector2D SpawnPos = Vector2D(cx / 2.0 + RandomClamped()*cx / 2.0,
								cy / 2.0 + RandomClamped()*cy / 2.0);

	  // If it's the firts FollowAgent, he follows the LeaderAgent
	  if (a == 0) {
		  Vehicle* pVehicle = new FollowAgent(this,
			  SpawnPos,                 //initial position
			  RandFloat()*TwoPi,        //start rotation
			  Vector2D(0, 0),            //velocity
			  Prm.VehicleMass,          //mass
			  Prm.MaxSteeringForce,     //max force
			  Prm.MaxSpeed,             //max velocity
			  Prm.MaxTurnRatePerSecond, //max turn rate
			  Prm.VehicleScale);        //scale

		  dynamic_cast<FollowAgent*>(pVehicle)->FlockingOn();
		  dynamic_cast<FollowAgent*>(pVehicle)->OnPursuit(pVehicleTemp, Vector2D(5,5));

		  pVehicleTemp = pVehicle;

		  m_Vehicles.push_back(pVehicle);

		  //add it to the cell subdivision
		  m_pCellSpace->AddEntity(pVehicle);

	  }
	  // Otherwise, he follows the FollowAgent in front of him.
	  else
	  {

		  Vehicle* pVehicle = new FollowAgent(this,
			  SpawnPos,                 //initial position
			  RandFloat()*TwoPi,        //start rotation
			  Vector2D(0, 0),            //velocity
			  Prm.VehicleMass,          //mass
			  Prm.MaxSteeringForce,     //max force
			  Prm.MaxSpeed,             //max velocity
			  Prm.MaxTurnRatePerSecond, //max turn rate
			  Prm.VehicleScale);        //scale

		  dynamic_cast<FollowAgent*>(pVehicle)->FlockingOn();
		  dynamic_cast<FollowAgent*>(pVehicle)->OnPursuit(pVehicleTemp, Vector2D(5, 5));

		  pVehicleTemp = pVehicle;

		  m_Vehicles.push_back(pVehicle);

		  //add it to the cell subdivision
		  m_pCellSpace->AddEntity(pVehicle);

	  }
	

  }


//#define SHOAL
//#ifdef SHOAL
  //m_Vehicles[Prm.NumAgents-1]->Steering()->FlockingOff();
  //m_Vehicles[Prm.NumAgents-1]->SetScale(Vector2D(10, 10));
  //m_Vehicles[Prm.NumAgents-1]->Steering()->WanderOn();
  //m_Vehicles[Prm.NumAgents-1]->SetMaxSpeed(70);


   //for (int i=0; i<Prm.NumAgents-1; ++i)
  //{
    //m_Vehicles[i]->Steering()->EvadeOn(m_Vehicles[Prm.NumAgents-1]);

  //}
//#endif
 
  //create any obstacles or walls
  //CreateObstacles();
  //CreateWalls();
}


//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
GameWorld::~GameWorld()
{
  for (unsigned int a=0; a<m_Vehicles.size(); ++a)
  {
    delete m_Vehicles[a];
  }

  for (unsigned int ob=0; ob<m_Obstacles.size(); ++ob)
  {
    delete m_Obstacles[ob];
  }

  delete m_pCellSpace;
  
  delete m_pPath;
}


//----------------------------- Update -----------------------------------
//------------------------------------------------------------------------
void GameWorld::Update(double time_elapsed)
{ 
  if (m_bPaused) return;

  //create a smoother to smooth the framerate
  const int SampleRate = 10;
  static Smoother<double> FrameRateSmoother(SampleRate, 0.0);

  m_dAvFrameTime = FrameRateSmoother.Update(time_elapsed);
  

  //update the vehicles
  for (unsigned int a=0; a<m_Vehicles.size(); ++a)
  {
	if (a == 2 && typeid(*m_Vehicles[1]).name() == "LeaderAgent") {
		Vector2D followAgentPos = m_Vehicles[a]->Pos();
		Vector2D leader1Pos = m_Vehicles[0]->Pos();
		Vector2D leader2Pos = m_Vehicles[1]->Pos();
		float dist1 = followAgentPos.Distance(leader1Pos);
		float dist2 = followAgentPos.Distance(leader2Pos);
		if (dist1 < dist2)
		{
			dynamic_cast<FollowAgent*>(m_Vehicles[a])->OffPursuit();
			dynamic_cast<FollowAgent*>(m_Vehicles[a])->OnPursuit(m_Vehicles[0], Vector2D(5, 5));
		}
		else
		{
			dynamic_cast<FollowAgent*>(m_Vehicles[a])->OffPursuit();
			dynamic_cast<FollowAgent*>(m_Vehicles[a])->OnPursuit(m_Vehicles[1], Vector2D(5, 5));
		}
	}
	m_Vehicles[a]->Update(time_elapsed);
  }
}
  

//--------------------------- CreateWalls --------------------------------
//
//  creates some walls that form an enclosure for the steering agents.
//  used to demonstrate several of the steering behaviors
//------------------------------------------------------------------------
void GameWorld::CreateWalls()
{
  //create the walls  
  double bordersize = 20.0;
  double CornerSize = 0.2;
  double vDist = m_cyClient-2*bordersize;
  double hDist = m_cxClient-2*bordersize;
  
  const int NumWallVerts = 8;

  Vector2D walls[NumWallVerts] = {Vector2D(hDist*CornerSize+bordersize, bordersize),
                                   Vector2D(m_cxClient-bordersize-hDist*CornerSize, bordersize),
                                   Vector2D(m_cxClient-bordersize, bordersize+vDist*CornerSize),
                                   Vector2D(m_cxClient-bordersize, m_cyClient-bordersize-vDist*CornerSize),
                                         
                                   Vector2D(m_cxClient-bordersize-hDist*CornerSize, m_cyClient-bordersize),
                                   Vector2D(hDist*CornerSize+bordersize, m_cyClient-bordersize),
                                   Vector2D(bordersize, m_cyClient-bordersize-vDist*CornerSize),
                                   Vector2D(bordersize, bordersize+vDist*CornerSize)};
  
  for (int w=0; w<NumWallVerts-1; ++w)
  {
    m_Walls.push_back(Wall2D(walls[w], walls[w+1]));
  }

  m_Walls.push_back(Wall2D(walls[NumWallVerts-1], walls[0]));
}


//--------------------------- CreateObstacles -----------------------------
//
//  Sets up the vector of obstacles with random positions and sizes. Makes
//  sure the obstacles do not overlap
//------------------------------------------------------------------------
void GameWorld::CreateObstacles()
{
    //create a number of randomly sized tiddlywinks
  for (int o=0; o<Prm.NumObstacles; ++o)
  {   
    bool bOverlapped = true;

    //keep creating tiddlywinks until we find one that doesn't overlap
    //any others.Sometimes this can get into an endless loop because the
    //obstacle has nowhere to fit. We test for this case and exit accordingly

    int NumTrys = 0; int NumAllowableTrys = 2000;

    while (bOverlapped)
    {
      NumTrys++;

      if (NumTrys > NumAllowableTrys) return;
      
      int radius = RandInt((int)Prm.MinObstacleRadius, (int)Prm.MaxObstacleRadius);

      const int border                 = 10;
      const int MinGapBetweenObstacles = 20;

      Obstacle* ob = new Obstacle(RandInt(radius+border, m_cxClient-radius-border),
                                  RandInt(radius+border, m_cyClient-radius-30-border),
                                  radius);

      if (!Overlapped(ob, m_Obstacles, MinGapBetweenObstacles))
      {
        //its not overlapped so we can add it
        m_Obstacles.push_back(ob);

        bOverlapped = false;
      }

      else
      {
        delete ob;
      }
    }
  }
}


//------------------------- Set Crosshair ------------------------------------
//
//  The user can set the position of the crosshair by right clicking the
//  mouse. This method makes sure the click is not inside any enabled
//  Obstacles and sets the position appropriately
//------------------------------------------------------------------------
void GameWorld::SetCrosshair(POINTS p)
{
  Vector2D ProposedPosition((double)p.x, (double)p.y);

  //make sure it's not inside an obstacle
  for (ObIt curOb = m_Obstacles.begin(); curOb != m_Obstacles.end(); ++curOb)
  {
    if (PointInCircle((*curOb)->Pos(), (*curOb)->BRadius(), ProposedPosition))
    {
      return;
    }

  }
  m_vCrosshair.x = (double)p.x;
  m_vCrosshair.y = (double)p.y;
}


//------------------------- HandleKeyPresses -----------------------------
void GameWorld::HandleKeyPresses(WPARAM wParam)
{

  switch(wParam)
  {
  case 'U':
    {
      delete m_pPath;
      double border = 60;
      m_pPath = new Path(RandInt(3, 7), border, border, cxClient()-border, cyClient()-border, true); 
      m_bShowPath = true; 
      for (unsigned int i=0; i<m_Vehicles.size(); ++i)
      {
        m_Vehicles[i]->Steering()->SetPath(m_pPath->GetPath());
      }
    }
    break;

    case 'P':
      
      TogglePause(); break;

    case 'O':

      ToggleRenderNeighbors(); break;

    case 'I':

      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->ToggleSmoothing();
        }

      }

      break;

    case 'Y':

       m_bShowObstacles = !m_bShowObstacles;

        if (!m_bShowObstacles)
        {
          m_Obstacles.clear();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOff();
          }
        }
        else
        {
          CreateObstacles();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOn();
          }
        }
        break;

	case 'Z':
		break;
	
	case 'S':
		break;

	case 'D':
		break;

	case 'Q':
		Vector2D target = m_Vehicles[0]->Pos();
		target.Normalize();
		double speedX = m_Vehicles[0]->Velocity().x;
		double speedY = m_Vehicles[0]->Velocity().y;

		if (target.x < 0)
		{
			if (target.y < 0)
			{
				target.x += speedX;
				target.y += -speedY;
			}
			else
			{
				target.x += -speedX;
				target.y += -speedY;
			}

		}
		else
		{
			if (target.y < 0)
			{
				target.x += speedX;
				target.y += speedY;
			}
			else
			{
				target.x += -speedX;
				target.y += speedY;
			}
		}

		target.Normalize();
		Vector2D Target = PointToWorldSpace(target,
			m_Vehicles[0]->Heading(),
			m_Vehicles[0]->Side(),
			m_Vehicles[0]->Pos());

		m_Vehicles[0]->SetPos(Target);
		break;

  }//end switch
}



//-------------------------- HandleMenuItems -----------------------------
void GameWorld::HandleMenuItems(WPARAM wParam, HWND hwnd)
{
  switch(wParam)
  {
    case ID_OB_OBSTACLES:

        m_bShowObstacles = !m_bShowObstacles;

        if (!m_bShowObstacles)
        {
          m_Obstacles.clear();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOff();
          }

          //uncheck the menu
         ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_UNCHECKED);
        }
        else
        {
          CreateObstacles();

          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_Vehicles[i]->Steering()->ObstacleAvoidanceOn();
          }

          //check the menu
          ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_CHECKED);
        }

       break;

    case ID_OB_WALLS:

      m_bShowWalls = !m_bShowWalls;

      if (m_bShowWalls)
      {
        CreateWalls();

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->WallAvoidanceOn();
        }

        //check the menu
         ChangeMenuState(hwnd, ID_OB_WALLS, MFS_CHECKED);
      }

      else
      {
        m_Walls.clear();

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->WallAvoidanceOff();
        }

        //uncheck the menu
         ChangeMenuState(hwnd, ID_OB_WALLS, MFS_UNCHECKED);
      }

      break;


    case IDR_PARTITIONING:
      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->ToggleSpacePartitioningOnOff();
        }

        //if toggled on, empty the cell space and then re-add all the 
        //vehicles
        if (m_Vehicles[0]->Steering()->isSpacePartitioningOn())
        {
          m_pCellSpace->EmptyCells();
       
          for (unsigned int i=0; i<m_Vehicles.size(); ++i)
          {
            m_pCellSpace->AddEntity(m_Vehicles[i]);
          }

          ChangeMenuState(hwnd, IDR_PARTITIONING, MFS_CHECKED);
        }
        else
        {
          ChangeMenuState(hwnd, IDR_PARTITIONING, MFS_UNCHECKED);
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
          m_bShowCellSpaceInfo = false;

        }
      }

      break;

    case IDM_PARTITION_VIEW_NEIGHBORS:
      {
        m_bShowCellSpaceInfo = !m_bShowCellSpaceInfo;
        
        if (m_bShowCellSpaceInfo)
        {
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_CHECKED);

          if (!m_Vehicles[0]->Steering()->isSpacePartitioningOn())
          {
            SendMessage(hwnd, WM_COMMAND, IDR_PARTITIONING, NULL);
          }
        }
        else
        {
          ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
        }
      }
      break;
        

    case IDR_WEIGHTED_SUM:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_CHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::weighted_average);
        }
		m_bWeightedSum = true;
      }

      break;

    case IDR_PRIORITIZED:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_CHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::prioritized);
        }

		m_bWeightedSum = false;
      }

      break;

    case IDR_DITHERED:
      {
        ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
        ChangeMenuState(hwnd, IDR_DITHERED, MFS_CHECKED);

        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::dithered);
        }

		m_bWeightedSum = false;
      }

      break;


      case ID_VIEW_KEYS:
      {
        ToggleViewKeys();

        CheckMenuItemAppropriately(hwnd, ID_VIEW_KEYS, m_bViewKeys);
      }

      break;

      case ID_VIEW_FPS:
      {
        ToggleShowFPS();

        CheckMenuItemAppropriately(hwnd, ID_VIEW_FPS, RenderFPS());
      }

      break;

      case ID_MENU_SMOOTHING:
      {
        for (unsigned int i=0; i<m_Vehicles.size(); ++i)
        {
          m_Vehicles[i]->ToggleSmoothing();
        }

        CheckMenuItemAppropriately(hwnd, ID_MENU_SMOOTHING, m_Vehicles[0]->isSmoothingOn());
      }

	  break;

	  case IDA_ONE_LEADER:
	  {
			  ChangeMenuState(hwnd, IDA_ONE_LEADER, MFS_CHECKED);
			  ChangeMenuState(hwnd, IDA_TWO_LEADER, MFS_UNCHECKED);
			  ChangeMenuState(hwnd, IDA_HUMAN_LEADER, MFS_UNCHECKED);

			  if (RenderTwoLeader()) 
			  {
				  m_Vehicles.erase(m_Vehicles.begin() + 1);
				  m_bTwoLeader = false;
			  }

			  if (RenderHumanLeader())
			  {

				  m_Vehicles.insert(m_Vehicles.begin(), pLeader_temp);
				  if (RenderTwoOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(2.0, 2.0));
				  if (RenderFiveOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(5.0, 5.0));
				  if (RenderTenOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(10.0, 10.0));
				  m_bHumanLeader = false;
			  }

			  m_bOneLeader = true;

	  }

	  break;

	  case IDA_TWO_LEADER:
	  {
		  ChangeMenuState(hwnd, IDA_ONE_LEADER, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDA_TWO_LEADER, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDA_HUMAN_LEADER, MFS_UNCHECKED);

		  if (RenderOneLeader()) 
		  {
			  //determine a random starting position
			  Vector2D SpawnPos = Vector2D(2.0, 2.0);

			  dynamic_cast<LeaderAgent*>(pSecondLeader_temp)->OnMoving();
			  m_Vehicles.insert(m_Vehicles.begin() + 1, pSecondLeader_temp);
			  m_bOneLeader = false;
		  }
		  if (RenderHumanLeader())
		  {
			  m_Vehicles.erase(m_Vehicles.begin());
			  m_Vehicles.insert(m_Vehicles.begin(), pLeader_temp);
			  if (RenderTwoOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(2.0, 2.0));
			  if (RenderFiveOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(5.0, 5.0));
			  if (RenderTenOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pLeader_temp, Vector2D(10.0, 10.0));
			  m_Vehicles.insert(m_Vehicles.begin() + 1, pSecondLeader_temp);
			  m_bHumanLeader = false;
		  }
		  	 

		  m_bTwoLeader = true;
	  }

	  break;

	  case IDA_HUMAN_LEADER:
	  {
		  ChangeMenuState(hwnd, IDA_ONE_LEADER, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDA_TWO_LEADER, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDA_HUMAN_LEADER, MFS_CHECKED);

		  if (RenderOneLeader())
		  {
			  m_Vehicles.erase(m_Vehicles.begin());
			  m_Vehicles.insert(m_Vehicles.begin(), pHumanLeader_temp);
			  if (RenderTwoOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(2.0, 2.0));
			  if (RenderFiveOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(5.0, 5.0));
			  if (RenderTenOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(10.0, 10.0));

			  m_bOneLeader = false;
		  }
		  if (RenderTwoLeader())
		  {
			  m_Vehicles.erase(m_Vehicles.begin());
			  m_Vehicles.erase(m_Vehicles.begin());
			  m_Vehicles.insert(m_Vehicles.begin(), pHumanLeader_temp);
			  if (RenderTwoOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(2.0, 2.0));
			  if (RenderFiveOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(5.0, 5.0));
			  if (RenderTenOffset()) m_Vehicles[1]->Steering()->OffsetPursuitOn(pHumanLeader_temp, Vector2D(10.0, 10.0));

			  m_bTwoLeader = false;
		  }
	  }
      break;

	  case IDN_TWENTY_AGENT:
	  {
		  ChangeMenuState(hwnd, IDN_TWENTY_AGENT, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDN_FIFTY_AGENT, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDN_HUNDRED_AGENT, MFS_UNCHECKED);

		  if (RenderFiftyAgent())
		  {
			  for (int i = 50; i > 20; i--)
			  {
				  
				  m_Vehicles[i]->Steering()->OffsetPursuitOff();
				  m_Vehicles.erase(m_Vehicles.begin() + i);
			  }
			  m_bFiftyAgent = false;

		  }
		  if (RenderHundredAgent())
		  {
			  for (int i = 100; i > 20; i--)
			  {
				  m_Vehicles[i]->Steering()->OffsetPursuitOff();
				  m_Vehicles.erase(m_Vehicles.begin() + i);
			  }
			  m_bHundredAgent = false;
		  }

		  m_bTwentyAgent = true;
	  }
	  break;

	  case IDN_FIFTY_AGENT:
	  {
		  ChangeMenuState(hwnd, IDN_TWENTY_AGENT, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDN_FIFTY_AGENT, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDN_HUNDRED_AGENT, MFS_UNCHECKED);

		  if (RenderTwentyAgent())
		  {
			  for (int i = 0; i < 30; i++) 
			  {
				  Vector2D SpawnPos = Vector2D(2.0, 2.0);

				  Vehicle* pVehicle = new FollowAgent(this,
					  SpawnPos,                 //initial position
					  RandFloat()*TwoPi,        //start rotation
					  Vector2D(0, 0),            //velocity
					  Prm.VehicleMass,          //mass
					  Prm.MaxSteeringForce,     //max force
					  Prm.MaxSpeed,             //max velocity
					  Prm.MaxTurnRatePerSecond, //max turn rate
					  Prm.VehicleScale);        //scale

				  if (RenderTwoOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(2, 2));
				  if (RenderFiveOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(5, 5));
				  if (RenderTenOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(10, 10));
				  m_Vehicles.push_back(pVehicle);

				  if (RenderWeightedSum())
				  {
					  for (unsigned int i = 0; i < m_Vehicles.size(); ++i)
					  {
						  m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::weighted_average);
					  }
				  }

				 
			  }
			  m_bTwentyAgent = false;
		  }
		  if (RenderHundredAgent())
		  {
			  for (int i = 100; i > 50; i--)
			  {
				  pFollow_Temp = m_Vehicles[i];
				  m_Vehicles.erase(m_Vehicles.begin() + i);
				  delete pFollow_Temp;
			  }
			  m_bHundredAgent = false;
		  }

		  m_bFiftyAgent = true;
	  }
	  break;

	  case IDN_HUNDRED_AGENT:
	  {
		  ChangeMenuState(hwnd, IDN_TWENTY_AGENT, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDN_FIFTY_AGENT, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDN_HUNDRED_AGENT, MFS_CHECKED);

		  if (RenderTwentyAgent())
		  {
			  for (int i = 0; i < 100; i++)
			  {
				  Vector2D SpawnPos = Vector2D(2.0, 2.0);

				  Vehicle* pVehicle = new FollowAgent(this,
					  SpawnPos,                 //initial position
					  RandFloat()*TwoPi,        //start rotation
					  Vector2D(0, 0),            //velocity
					  Prm.VehicleMass,          //mass
					  Prm.MaxSteeringForce,     //max force
					  Prm.MaxSpeed,             //max velocity
					  Prm.MaxTurnRatePerSecond, //max turn rate
					  Prm.VehicleScale);        //scale

				  if (RenderTwoOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(2, 2));
				  if (RenderFiveOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(5, 5));
				  if (RenderTenOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(10, 10));
				  m_Vehicles.push_back(pVehicle);

				  if (RenderWeightedSum())
				  {
					  for (unsigned int i = 0; i < m_Vehicles.size(); ++i)
					  {
						  m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::weighted_average);
					  }
				  }

			  }
			  m_bTwentyAgent = false;
		  }

		  if (RenderFiftyAgent())
		  {
			  for (int i = 0; i < 50; i++)
			  {
				  Vector2D SpawnPos = Vector2D(2.0, 2.0);

				  Vehicle* pVehicle = new FollowAgent(this,
					  SpawnPos,                 //initial position
					  RandFloat()*TwoPi,        //start rotation
					  Vector2D(0, 0),            //velocity
					  Prm.VehicleMass,          //mass
					  Prm.MaxSteeringForce,     //max force
					  Prm.MaxSpeed,             //max velocity
					  Prm.MaxTurnRatePerSecond, //max turn rate
					  Prm.VehicleScale);        //scale

				  if (RenderTwoOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(2.0, 2.0));
				  if (RenderFiveOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(5.0, 5.0));
				  if (RenderTenOffset()) pVehicle->Steering()->OffsetPursuitOn(m_Vehicles[m_Vehicles.size()-1], Vector2D(10.0, 10.0));
				  m_Vehicles.push_back(pVehicle);

				  if (RenderWeightedSum())
				  {
					  for (unsigned int i = 0; i < m_Vehicles.size(); ++i)
					  {
						  m_Vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::weighted_average);
					  }
				  }


			  }
			  m_bFiftyAgent = false;
		  }

		  m_bHundredAgent = true;
	  }
	  break;

	  case IDO_TWO_OFFSET:
	  {
		  ChangeMenuState(hwnd, IDO_TWO_OFFSET, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDO_FIVE_OFFSET, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDO_TEN_OFFSET, MFS_UNCHECKED);

		  if (RenderFiveOffset())
		  {
			  if (RenderTwentyAgent())
			  {
				  for (int i = 1; i < 20; i++)
				  {
					  if (i == 0)
					  {

					  }
				  }
			  }
		  }
		  if (RenderTenOffset())
		  {

		  }

		  m_bTwoOffset = true;
	  }

	  break;

	  case IDO_FIVE_OFFSET:
	  {
		  ChangeMenuState(hwnd, IDO_TWO_OFFSET, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDO_FIVE_OFFSET, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDO_TEN_OFFSET, MFS_UNCHECKED);

		  m_bFiveOffset = true;
	  }

	  break;

	  case IDO_TEN_OFFSET:
	  {
		  ChangeMenuState(hwnd, IDO_TWO_OFFSET, MFS_UNCHECKED);
		  ChangeMenuState(hwnd, IDO_FIVE_OFFSET, MFS_CHECKED);
		  ChangeMenuState(hwnd, IDO_TEN_OFFSET, MFS_UNCHECKED);

		  m_bTenOffset = true;
	  }

	  break;

  }//end switch
}


//------------------------------ Render ----------------------------------
//------------------------------------------------------------------------
void GameWorld::Render()
{
  gdi->TransparentText();

  //render any walls
  gdi->BlackPen();
  for (unsigned int w=0; w<m_Walls.size(); ++w)
  {
    m_Walls[w].Render(true);  //true flag shows normals
  }

  //render any obstacles
  gdi->BlackPen();
  
  for (unsigned int ob=0; ob<m_Obstacles.size(); ++ob)
  {
    gdi->Circle(m_Obstacles[ob]->Pos(), m_Obstacles[ob]->BRadius());
  }

  //render the agents
  for (unsigned int a=0; a<m_Vehicles.size(); ++a)
  {
    m_Vehicles[a]->Render(); 
    
    //render cell partitioning stuff
    if (m_bShowCellSpaceInfo && a==0)
    {
      gdi->HollowBrush();
      InvertedAABBox2D box(m_Vehicles[a]->Pos() - Vector2D(Prm.ViewDistance, Prm.ViewDistance),
                           m_Vehicles[a]->Pos() + Vector2D(Prm.ViewDistance, Prm.ViewDistance));
      box.Render();

      gdi->RedPen();
      CellSpace()->CalculateNeighbors(m_Vehicles[a]->Pos(), Prm.ViewDistance);
      for (BaseGameEntity* pV = CellSpace()->begin();!CellSpace()->end();pV = CellSpace()->next())
      {
        gdi->Circle(pV->Pos(), pV->BRadius());
      }
      
      gdi->GreenPen();
      gdi->Circle(m_Vehicles[a]->Pos(), Prm.ViewDistance);
    }
  }  

//#define CROSSHAIR
#ifdef CROSSHAIR
  //and finally the crosshair
  gdi->RedPen();
  gdi->Circle(m_vCrosshair, 4);
  gdi->Line(m_vCrosshair.x - 8, m_vCrosshair.y, m_vCrosshair.x + 8, m_vCrosshair.y);
  gdi->Line(m_vCrosshair.x, m_vCrosshair.y - 8, m_vCrosshair.x, m_vCrosshair.y + 8);
  gdi->TextAtPos(5, cyClient() - 20, "Click to move crosshair");
#endif


  //gdi->TextAtPos(cxClient() -120, cyClient() - 20, "Press R to reset");

  gdi->TextColor(Cgdi::grey);
  if (RenderPath())
  {
     gdi->TextAtPos((int)(cxClient()/2.0f - 80), cyClient() - 20, "Press 'U' for random path");

     m_pPath->Render();
  }

  if (RenderFPS())
  {
    gdi->TextColor(Cgdi::grey);
    gdi->TextAtPos(5, cyClient() - 20, ttos(1.0 / m_dAvFrameTime));
  } 

  if (m_bShowCellSpaceInfo)
  {
    m_pCellSpace->RenderCells();
  }

}
