#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"
#include "Base.h"

using namespace sc2;

class CombatManager : public Manager {
public:
	CombatManager();

	void OnIdleMarine(const Unit* unit);
	void OnIdleReaper(const Unit* unit);

	void CheckForEnemyBase();

	bool AttackEnemy();
private:

	Point2D enemyStartLocation;
	std::map<const Unit*, Point2D> scoutingMarines;

	size_t numberIdleMarines = 0;
};