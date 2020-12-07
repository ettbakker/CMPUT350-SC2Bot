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

/**
* SAM_Sc2Bot is the main bot class responsible for defining the general behavior of the AI while delegating
* specific decision-making and lower-level operations to manager classes.
* 
* Most manager operations are called on each in-game step while others are called only after some amount of
* steps have elapsed since the start of the game (e.g. adding new bases, attacking the enemy base).
*/

class SAM_Sc2Bot : public sc2::Agent {
public:
	// Initialize managers, bases, and possible expansion locations
	virtual void OnGameStart();

	// During each in-game step, attempt to build structures and attack enemies near the home base.
	// After a certain amount of in-game time, expand our base and later attack the enemy's base.
	virtual void OnStep();

	// Delegates OnIdle functions to appropriate managers.
	virtual void OnUnitIdle(const Unit* unit);

	// Print the results of the players and deallocate managers and bases on game over.
	virtual void OnGameEnd();

private:
	// Add a new base location by choosing the next closest expansion location we haven't already used.
	// Only a maximum of 3 bases are allowed, so max 2 expansions from the home base.
	bool AddBase();

	//Sort the expansion location in order of closest to main base using simple bubble sort
	void SortExpansionLocations();

	ProductionManager* prodMngr;
	CombatManager* combatMngr;
	Bases bases;
	std::vector<Point3D> expansionLocations;
	size_t step_count = 0;
};

#endif