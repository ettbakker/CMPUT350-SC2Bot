#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();
	bases.push_back(new Base(Observation()->GetStartLocation()));
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

	prodMngr->TryBuildSupplyDepot();
	prodMngr->TryBuildRefinery();
	prodMngr->TryBuildCommandCenter();
	prodMngr->TryBuildBarracks();
	prodMngr->TryBuildEngineeringBay();
	prodMngr->TryBuildTurrets();
	prodMngr->TryBuildArmory();
	prodMngr->TryBuildFactory();
	//prodMngr->TryBuildStarPort();
	//prodMngr->TryBuildFusionCore();
	//prodMngr->fixBuildings();

	//resetBasesNumber();
	
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	prodMngr->SetObservationAndActions(Observation(), Actions(), bases);
	combatMngr->SetObservationAndActions(Observation(), Actions(), bases);

	switch (unit->unit_type.ToType()) {
		case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		{
			prodMngr->OnIdleCommandCenter(unit); 
			break;
		}

		case UNIT_TYPEID::TERRAN_SCV:
		{
			prodMngr->OnIdleSCV(unit); 
			break;
		}

		case UNIT_TYPEID::TERRAN_BARRACKS:
		{
			prodMngr->OnIdleBarracks(unit);  break;
		}

		case UNIT_TYPEID::TERRAN_MARINE:
		{
			combatMngr->OnIdleMarine(unit); break;
		}
		case UNIT_TYPEID::TERRAN_REAPER:
		{
			combatMngr->OnIdleReaper(unit); break;
		}
		default:
		{
			break;
		}
	}
}

void BasicSc2Bot::resetBasesNumber() {

	if (resetBasesNumberInNumSteps == 20) {
		Units commandCenters = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER));
		for (int i = 0; i < bases.size(); i++) {
			bool stillABase = false;
			for (auto c : commandCenters) {
				//There is still a command center near that base location
				if (DistanceSquared2D(c->pos, bases[i]->origin) < 10) {
					stillABase = true;
					break;
				}
			}
			//There is no command center near this base so remove it from our base list
			if (!stillABase) {
				std::cout << "base erased" << std::endl;
				bases.erase(bases.begin() + i);
			}
		}
	}
	else if (resetBasesNumberInNumSteps == 50) {
		resetBasesNumberInNumSteps = -1;
	}
	++resetBasesNumberInNumSteps;
}