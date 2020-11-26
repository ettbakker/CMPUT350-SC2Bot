#pragma once
#include "Command.h"
#include "CombatManager.h"
#include "ProductionManager.h"
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <vector>

using namespace sc2;

class StrategyManager : public Agent {
public:
	StrategyManager();
	~StrategyManager();
	bool GetNextCommand(Command& next_cmd);
	void PlanOnIdle(const Unit* unit, CommandSequence& idle_cmds);
	void PlanStep();
private:
	ProductionManager* prodMngr;
	CombatManager* combatMngr;
	CommandSequence* stepPlan;
};