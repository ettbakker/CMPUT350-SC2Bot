#include "StrategyManager.h"
#include "sc2api/sc2_api.h"

StrategyManager::StrategyManager()
{
	prodMngr = new ProductionManager();
	combatMngr = new CombatManager();
	stepPlan = new CommandSequence();
}

StrategyManager::~StrategyManager()
{
	delete prodMngr;
	delete combatMngr;
}

void StrategyManager::PlanStep() {
	//Actions()->UnitCommand();
}

bool StrategyManager::GetNextCommand(Command& next_cmd) {
	if (!stepPlan->IsEmpty()) {
		next_cmd = stepPlan->Front();
		stepPlan->PopFront();
	}

	return false;
}

void StrategyManager::PlanOnIdle(const Unit* unit, CommandSequence& idle_cmds) {

}
