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
	void CalculateGatherLocation();
	void GatherNearEnemy();
	void OnIdleMarine(const Unit* unit);
	void OnIdleReaper(const Unit* unit);
	bool FindEnemyBase();
	void sortAndAddSweepLocations(Point2D fromPoint);

	void HellionMorph(const Units units, bool attacking, Point2D to);
	
private:
	size_t numberIdleMarines = 0;
	Point2D enemyStartLocation, lastAllOutPos, gatherLocation;
	bool allOutAttack = false;
	bool foundEnemyBase = false;
	std::map<const Unit*, Point2D> scoutingMarines;
	bool updateSweeping(Units &army, Units &enemies);
	std::vector<Point2D> sweepLocations;
	size_t sweepLocationCounter = 0;
	size_t numberTimesSinceNewTarget = 0;
	const Unit* targetAtBase = nullptr;
	bool defendBase = false;
	size_t numEnemyAtBase = 0;
};