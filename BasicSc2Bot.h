#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>

using namespace sc2;

class BasicSc2Bot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnUnitIdle(const Unit* unit);
	virtual void OnGameOver();

private:
	//Multiple-use Functions
	size_t CountUnitType(UNIT_TYPEID unit_type);
	bool TryBuildStructure(ABILITY_ID ability_type_for_structure, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
	bool TryBuildStructureAtPoint(ABILITY_ID ability_type_for_structure, Point2D point, UNIT_TYPEID unit_type = UNIT_TYPEID::TERRAN_SCV);
	const Unit* FindNearestUnit(const Point2D& start, UNIT_TYPEID unit_type);

	//Build Buildings
	bool TryBuildSupplyDepot();
	bool TryBuildRefinery();
	bool TryBuildCommandCenter();
	bool TryBuildBarracks();
	bool TryBuildEngineeringBay();
	bool TryBuildTurrets();

	//Attacking
	bool AttackEnemy();
	bool fixBuildings();

	//Variables
	size_t randomMarineLocation = 0;
	bool enemySpotted = false;
};

#endif