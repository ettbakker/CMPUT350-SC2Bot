#include "StrategyManager.h"

StrategyManager::StrategyManager()
{
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();
}

StrategyManager::~StrategyManager()
{
	delete prodMngr;
	delete combatMngr;
}

void StrategyManager::planStep(CommandSequence& step_commands) 
{

}

bool StrategyManager::getNextCommand(Command& next_cmd) {
	if (!plan.isEmpty()) {
		next_cmd = plan.front();
		plan.popFront();
	}

	return false;
}
