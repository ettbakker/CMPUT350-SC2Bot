#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	strategyManager = new StrategyManager();

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

}

void BasicSc2Bot::ExecuteCommand(const Command& cmd) {
	Units units = cmd.get_units();
	AbilityID ability = cmd.get_ability();
	const Unit* target;
	Point2D point;

	if (cmd.get_point(point)) {
		Actions()->UnitCommand(units, ability, point);
	} 
	else if (cmd.get_target(&target)) {
		Actions()->UnitCommand(units, ability, target);
	}
	else {
		Actions()->UnitCommand(units, ability);
	}
}



void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	
}