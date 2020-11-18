#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "Command.h"
#include "StrategyManager.h"
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>
#include <algorithm>

using namespace sc2;

class BasicSc2Bot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);
	virtual void OnGameOver();
	void ExecuteCommand(const Command& cmd);

private:

	// Variables
	StrategyManager* strategyManager;
	size_t randomMarineLocation = 0;
	bool enemySpotted = false;
};

#endif