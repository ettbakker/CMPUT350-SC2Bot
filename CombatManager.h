#pragma once
#include "sc2api/sc2_api.h"
#include <unordered_map>
#include "TerranUnitCategories.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"
#include "Base.h"

using namespace sc2;

class CombatManager : public Manager {
public:
	CombatManager();

	void OnIdleMarine(const Unit* unit);
	void OnIdleSmart(const Unit* unit, UNIT_TYPEID unit_type);
	bool AttackEnemy();
	void InitArmyCounts();
	void UpdateArmyCounts();
	size_t GetArmySize();
private:
	size_t numberIdleMarines = 0;
	std::unordered_map<UNIT_TYPEID, size_t> army_counts;
};