#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Base.h"
#include "TerranUnitCategories.h"
#include <list>

using namespace sc2;

/**
* The Combat Manager is responsible for all military operations including the scouting/attacking of enemy base(s), the defense
* of our base(s), and the behavior of combat units like Marines and Hellions.
*/

class CombatManager : public Manager {
public:
	CombatManager();

	// Called each step to attack enemies that approach our base. 
	// If we're not attacking the enemy base, focus our army units on attacking only enemies that come too close.
	// Otherwise, either dispatch a squad of marines to defend our base or recall the entire army if too many enemies
	// are attacking our base.
	bool AttackEnemy();

	// Manages the process of deploying our entire army to attack the main enemy base.
	bool AllOutAttackEnemy();

	// Set the gathering location for our army prior to an all-out attack on the enemy base.
	void CalculateGatherLocation();

	//Accumulate the army somewhere near the enemy base. Helps with attacking at the same time.
	void GatherNearEnemy();

	// Send occasional scouts to look for enemies and attack if we have a small army.
	void OnIdleMarine(const Unit* unit);

	// Unimplemented as only Marines required OnIdle behavior for our bot's strategy.
	void OnIdleReaper(const Unit* unit);

	// Find the enemy's starting location by sending 1 marine to scout each possible enemy starting location.
	bool FindEnemyBase();

	// Sort sweeping locations from closest to enemy base location. When we run out of enemies to attack
	// at one location, our army will repeatedly sweep the nearest location next.
	void sortAndAddSweepLocations(Point2D fromPoint);

	//Sweeping lets the army go from location to location to find other enemy bases.
	//The sweeping locations are sorted from closest to enemy base which was determined at the start of the base
	//If too many locations have been sweeped with no enemies, we start sending reapers back to random locations since we
	//most likely missed the last few buildings.
	bool updateSweeping(Units& army, Units& enemies);

	// Given a vector of hellions/hellbats, morph them to their alternate form 
	// depending on if we're attacking.
	void HellionMorph(const Units units, bool attacking, Point2D to);
	
private:
	std::vector<Point2D> sweepLocations;
	std::map<const Unit*, Point2D> scoutingMarines;
	const Unit* targetAtBase = nullptr;

	Point2D enemyStartLocation, lastAllOutPos, gatherLocation;
	size_t numberIdleMarines = 0, sweepLocationCounter = 0,
		numberTimesSinceNewTarget = 0, numEnemyAtBase = 0;
	bool allOutAttack = false, foundEnemyBase = false,
		defendBase = false;
};