#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"
#include "Base.h"
#include "TerranUnitCategories.h"
#include <list>

using namespace sc2;

class CombatManager : public Manager {
public:
	CombatManager();

	bool AttackEnemy();
	bool AllOutAttackEnemy();
	void OnIdleMarine(const Unit* unit);
	void OnIdleReaper(const Unit* unit);
	
private:
	size_t numberIdleMarines = 0;
	Point2D enemyStartLocation, lastAllOutPos;
	bool allOutAttack = false;
	bool foundEnemyBase = false;
	std::vector<UNIT_TYPEID> army_unit_types = { UNIT_TYPEID::TERRAN_MARINE,
			UNIT_TYPEID::TERRAN_REAPER,
			UNIT_TYPEID::TERRAN_MARAUDER,
			UNIT_TYPEID::TERRAN_HELLION,
			UNIT_TYPEID::TERRAN_HELLIONTANK };
};