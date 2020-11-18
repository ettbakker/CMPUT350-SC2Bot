#pragma once
#include "Command.h"
#include "CombatManager.h"
#include "ProductionManager.h"
#include <vector>


class StrategyManager {
public:
	StrategyManager();
	~StrategyManager();
	bool getNextCommand(Command& next_cmd);
	void planStep(CommandSequence& step_commands);
private:
	ProductionManager* prodMngr;
	CombatManager* combatMngr;
	CommandSequence plan;
};