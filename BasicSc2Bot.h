#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>
#include "Builder.h"
#include "BotFunctions.h"

using namespace sc2;

class BasicSc2Bot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);
	virtual void OnGameOver();
	size_t CountUnitType(UNIT_TYPEID unit_type);

private:
	
	bool ManageSCV();
	bool GiveSCVACommand(const Unit* unit);

	//Attacking
	bool AttackEnemy();
	bool fixBuildings();
	void resetBasesNumber();
	

	//Variables
	size_t numberIdleMarines = 0;
	bool enemySpotted = false;
	BotFunctions *botFunctions = new BotFunctions();
	Builder *builder = new Builder();

	//Bases
	std::vector<Point2D> bases;
	int resetBasesNumberInNumSteps = 0;

};

#endif