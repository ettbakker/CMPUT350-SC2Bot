#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include <sc2lib/sc2_search.h>
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include "Manager.h"
#include "ProductionManager.h"
#include "CombatManager.h"
#include <iostream>
#include <algorithm>

using namespace sc2;

class BasicSc2Bot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);
	virtual void OnGameEnd();

private:
	// Variables
	ProductionManager* prodMngr;
	CombatManager* combatMngr;
	size_t randomMarineLocation = 0;
	bool enemySpotted = false;
	Bases bases;
	bool AddBase();
	std::vector<Point3D> expansionLocations;
	void SortExpansionLocations();
	size_t thirdBaseAttempt = 0;
};

#endif