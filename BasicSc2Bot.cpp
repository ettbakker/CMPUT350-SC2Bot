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
	prodMngr->TryBuildSupplyDepot();
	prodMngr->TryBuildRefinery();
	prodMngr->TryBuildCommandCenter();
	prodMngr->TryBuildBarracks();
	prodMngr->TryBuildEngineeringBay();
	prodMngr->TryBuildTurrets();
	prodMngr->TryBuildFactory();
	prodMngr->TryBuildStarPort();
	prodMngr->TryBuildFusionCore();
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{

}

/**
void BasicSc2Bot::ExecuteCommand(const Command& cmd) {
	Units units = cmd.GetUnits();
	AbilityID ability = cmd.GetAbility();
	const Unit* target;
	Point2D point;

	if (cmd.GetPoint(point)) {
		Actions()->UnitCommand(units, ability, point);
	} 
	else if (cmd.GetTarget(&target)) {
		Actions()->UnitCommand(units, ability, target);
	}
	else {
		Actions()->UnitCommand(units, ability);
	}
}
*/