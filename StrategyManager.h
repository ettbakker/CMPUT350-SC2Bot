#pragma once
#include "Command.h"
#include "Manager.h"
#include "CombatManager.h"
#include "ProductionManager.h"
#include "BasicSc2Bot.h"
#include <vector>

class StrategyManager : public Manager {
public:
	StrategyManager(BasicSc2Bot* bot);
	~StrategyManager();
	void planStep(std::vector<Command>& step_commands);
private:
	ProductionManager* prodMngr;
	CombatManager* combatMngr;
};