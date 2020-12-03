#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();
	bases.push_back(new Base(Observation()->GetStartLocation()));
	expansionLocations = search::CalculateExpansionLocations(Observation(), Query(), search::ExpansionParameters());
	SortExpansionLocations();
	std::cout << "Start X: " << bases[0]->origin.x << " Y: " << bases[0]->origin.y << std::endl;
	std::cout << "We have bases? " << bases.size() << "\n";
	std::cout << "hello, World!" << std::endl;
	return; 
}

//Print the results of the players once the game is over.
void BasicSc2Bot::OnGameEnd() {
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
	AddBase();
	
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
			
			prodMngr->OnIdleSCV(unit);
			
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
			prodMngr->OnIdleSCV(unit); break;
		}

		default:
		{
			break;
		}
	}
}



//Sort the expansion location in order of closest to main base
//Simple bubble sort
void BasicSc2Bot::SortExpansionLocations() {
	Point3D tempPoint;
	Point2D point1;
	Point2D point2;
	int distance1;
	int distance2;
	bool stillSwapping = false;
	std::cout << "Exp size: " << expansionLocations.size() << std::endl;
	for (size_t i = 0; i < expansionLocations.size(); i++) {
		for (size_t j = 0; j < expansionLocations.size()-i-1; j++) {
			point1 = Point2D(expansionLocations[j].x, expansionLocations[j].y);
			point2 = Point2D(expansionLocations[j + 1].x, expansionLocations[j + 1].y);
			distance1 = Distance2D(Observation()->GetStartLocation(), point1);
			distance2 = Distance2D(Observation()->GetStartLocation(), point2);
			if (distance1 > distance2) {
				//Swap them
				tempPoint = expansionLocations[j];
				expansionLocations[j] = expansionLocations[j + 1];
				expansionLocations[j + 1] = tempPoint;
				stillSwapping = true;
			}
		}
		if (!stillSwapping) {
			break;
		}
	}
}

//Add a new base. Can't be in another class since they can't change bases
bool BasicSc2Bot::AddBase() {
	
	//Bases are already sorted so add next expansion location that isn't already in our bases
	if (bases.size() < 4) {
		if (bases.size() < expansionLocations.size()) {
			Point2D newLocation = Point2D(expansionLocations[bases.size()-1].x, expansionLocations[bases.size()-1].y);
			bases.push_back(new Base(newLocation));
			std::cout << "Added a new base " << "X: " << newLocation.x << " Y: " << newLocation.y << std::endl;
			return true;
		}
	}
	return false;
}





