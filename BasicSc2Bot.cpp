#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();

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
}

void BasicSc2Bot::OnStep()
{
	prodMngr->SetObservationAndActions(Observation(), Actions());
	combatMngr->SetObservationAndActions(Observation(), Actions());

	prodMngr->TryBuildSupplyDepot();
	prodMngr->TryBuildRefinery();
	prodMngr->TryBuildCommandCenter();
	prodMngr->TryBuildBarracks();
	prodMngr->TryBuildEngineeringBay();
	prodMngr->TryBuildTurrets();
	prodMngr->TryBuildFactory();
	prodMngr->TryBuildStarPort();
	prodMngr->TryBuildFusionCore();
	prodMngr->TryRepairBuildings();
	combatMngr->CheckForEnemyBase();
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	prodMngr->SetObservationAndActions(Observation(), Actions());
	combatMngr->SetObservationAndActions(Observation(), Actions());
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

/**
void BasicSc2Bot::RunManagerCommands(Manager* mngr) {
	Command cmd;
	
	while (!(mngr->commands.IsEmpty())) {
		cmd = mngr->commands.Front();
		ExecuteCommand(cmd);
		mngr->commands.PopFront();
	}
}

void BasicSc2Bot::ExecuteCommand(const Command& cmd) {
	const Unit* unit = cmd.GetUnit();
	AbilityID ability = cmd.GetAbility();
	const Unit* target;
	Point2D point;
	
	if (cmd.GetPoint(point)) {
		std::cout << "Doing cmd with point\n";
		Actions()->UnitCommand(unit, ability, point);
	} 
	else if (cmd.GetTarget(&target)) {
		std::cout << "Cmd with target unit\n";
		Actions()->UnitCommand(unit, ability, target);
	}
	else {
		std::cout << "Cmd with only ability\n";
		Actions()->UnitCommand(unit, ability);
	}
}
*/