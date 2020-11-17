#include "StrategyManager.h"

StrategyManager::StrategyManager(BasicSc2Bot* bot) : Manager(bot)
{
	prodMngr = new ProductionManager(bot);
	combatMngr = new CombatManager(bot);
}

StrategyManager::~StrategyManager()
{
	delete prodMngr;
	delete combatMngr;
}

void StrategyManager::planStep(std::vector<Command>& step_commands) 
{

}
