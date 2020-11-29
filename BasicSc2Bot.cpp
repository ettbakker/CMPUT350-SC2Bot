#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();
	bases.push_back(new Base(Observation()->GetStartLocation()));
	std::cout << "Start X: " << bases[0]->origin.x << " Y: " << bases[0]->origin.y << std::endl;
	std::cout << "We have bases?" << bases.size() << "\n";
	std::cout << "hello, World!" << std::endl;
	return; 
}

//Print the results of the players once the game is over.
void BasicSc2Bot::OnGameOver() {
	std::vector<PlayerResult> res = Observation()->GetResults();
	for(auto r : res){
		std::cout << r.player_id << r.result << std::endl;
	}
	std::cout << "Game Over" << std::endl;

	delete prodMngr;
	delete combatMngr;
	for (auto b : bases) {
		delete b;
	}
}

void BasicSc2Bot::OnStep()
{
	prodMngr->SetObservationAndActions(Observation(), Actions(), bases);
	combatMngr->SetObservationAndActions(Observation(), Actions(), bases);
	prodMngr->BuildStructures();
	combatMngr->AttackEnemy();
	
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	prodMngr->SetObservationAndActions(Observation(), Actions(), bases);
	combatMngr->SetObservationAndActions(Observation(), Actions(), bases);

	switch (unit->unit_type.ToType()) {
		// buildings
		case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		{
			prodMngr->OnIdleCommandCenter(unit); 
			break;
		}

		case UNIT_TYPEID::TERRAN_BARRACKS:
		{
			prodMngr->OnIdleBarracks(unit);  break;
		}

		case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
		{
			prodMngr->OnIdleEngineeringBay(unit);  break;
		}

		case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
		{
			prodMngr->OnIdleOrbitalCommand(unit);  break;
		}

		// units
		case UNIT_TYPEID::TERRAN_SCV:
		{
			if (!AddBase(unit)) {
				prodMngr->OnIdleSCV(unit);
			}
			break;
		}

		case UNIT_TYPEID::TERRAN_MARINE:
		{
			combatMngr->OnIdleMarine(unit); break;
		}

		case UNIT_TYPEID::TERRAN_REAPER:
		{
			combatMngr->OnIdleReaper(unit); break;
		}

		case UNIT_TYPEID::TERRAN_MULE:
		{
			if (!AddBase(unit)) {
				prodMngr->OnIdleSCV(unit);
			}
			break;
		}

		default:
		{
			break;
		}
	}
}

//Add a new base. Can't be in another class since they can't change bases
//Very strange balance with the numbers needed to get it to build more than 2 bases
//Adding a 3rd base right now works only sometimes, since I'm trying not to build bases too close together.
bool BasicSc2Bot::AddBase(const Unit* unit) {
	if ((bases.size() < 3) && (thirdBaseAttempt<20)) {
		int j = 0;
		Point2D potentialLocation;
		float rx;
		float ry;
		bool suitable;
		//Keep track of how many times we try to add a third base so we don't do this forever
		//Sometimes it can't find a location
		if (bases.size() == 2) {
			++thirdBaseAttempt;
		}
		std::cout << "Searching for base " << bases.size()+1 << std::endl;
		while (j < 50) {
			++j;
			rx = GetRandomScalar();
			ry = GetRandomScalar();
			potentialLocation = Point2D(bases[0]->origin.x + rx * 42.0f, bases[0]->origin.y + ry * 42.0f);
			suitable = true;
			//Check that its not too close to another base
			if (bases.size() >=2) {
				for (int l = 1; l < bases.size(); l++) {
					if (Distance2D(potentialLocation, bases[l]->origin) < 30) {
						suitable = false;
						break;
					}
				}
			}
			
			//Check that its not near the edges of the map and somewhat further from the main base
			if ((DistanceSquared2D(potentialLocation, bases[0]->origin) > 450) 
				&& (potentialLocation.x > 20) && (potentialLocation.y > 20) 
				&& (potentialLocation.x < 145) && (potentialLocation.y < 145)
				&& suitable) {
				Actions()->UnitCommand(unit, ABILITY_ID::BUILD_COMMANDCENTER, potentialLocation);
				//prodMngr->TryBuildCommandCenter(50.0);
				bases.push_back(new Base(potentialLocation));
				std::cout << "Added a new base " << "X: " << potentialLocation.x << " Y: " << potentialLocation.y << std::endl;
				return true;
			}
		}
	}
	if (thirdBaseAttempt == 20) {
		std::cout << "Gave up looking for third base" << std::endl;
		++thirdBaseAttempt;
	}
	return false;
}





