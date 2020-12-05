#include "CombatManager.h"
#include "D:/Documents/TerranUnitCategories.h"

CombatManager::CombatManager()
{

}

bool CombatManager::AttackEnemy() {
	Units enemies = observation->GetUnits(Unit::Alliance::Enemy);
	const GameInfo& game_info = observation->GetGameInfo();
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	bool attack = false;
	//For all enemies, attack the one that is closest to the base
	for (auto e : enemies) {
		for (auto b : bases) {
			float d = DistanceSquared2D(e->pos, b->origin);
			if (d < distance)
			{
				distance = d;
				target = e;
			}
			if (d < 1000) {
				attack = true;
			}
		}
	}
	//Send all units to the enemy
	Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
	Units marines = observation->GetUnits(Unit::Alliance::Self, filter);
	filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
	Units reapers = observation->GetUnits(Unit::Alliance::Self, filter);
	filter = IsUnit(UNIT_TYPEID::TERRAN_MARAUDER);
	Units marauder = observation->GetUnits(Unit::Alliance::Self, filter);
	filter = IsUnit(UNIT_TYPEID::TERRAN_SIEGETANK);
	Units tank = observation->GetUnits(Unit::Alliance::Self, filter);
	filter = IsUnit(UNIT_TYPEID::TERRAN_HELLION);
	Units hellion = observation->GetUnits(Unit::Alliance::Self, filter);

	if (marines.size() > 1) {
		if (attack && (target != nullptr)) {
			//TODO: Allow other units to attack
			//We have already spotted an enemy
			actions->UnitCommand(marines, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(reapers, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(marauder, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(tank, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(hellion, ABILITY_ID::ATTACK_ATTACK, target);
			return true;
		} else if ((observation->GetArmyCount() > 80) && (target != nullptr)) {
			//TODO: Allow other units to attack
			//We have already spotted an enemy
			actions->UnitCommand(marines, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(reapers, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(marauder, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(tank, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(hellion, ABILITY_ID::ATTACK_ATTACK, target);
			return true;
		}
		else if (observation->GetArmyCount() > 80) { 
			//We need to find an enemy location so choose a random expansion location or start location
			Point2D attackPoint = observation->GetStartLocation();
			size_t randomLoc = 0;
			if ((rand() % 2) == 0) {
				randomLoc = rand() % game_info.enemy_start_locations.size();
				attackPoint = game_info.enemy_start_locations[randomLoc];
			}
			else {
				randomLoc = rand() % expansionLocations.size();
				attackPoint = expansionLocations[randomLoc];
			}
			//Attack the location
			//actions->UnitCommand(reapers, ABILITY_ID::ATTACK, attackPoint,true);
			//actions->UnitCommand(marauder, ABILITY_ID::ATTACK, attackPoint,true);
			//actions->UnitCommand(tank, ABILITY_ID::ATTACK, attackPoint,true);
			if (CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
				actions->UnitCommand(hellion[0], ABILITY_ID::ATTACK, attackPoint, true);
			}
			else {
				actions->UnitCommand(marines[0], ABILITY_ID::ATTACK, attackPoint,true);
			}
			
			return true;

		}

		if ((target != nullptr) && observation->GetArmyCount() < 30){
			//If there is a target and they are far away, call back all units so they don't keep chasing it
			//because our army is still small
			//Recall to nearest command center
			//**CRASHINGconst Unit *unitPoint = GetNearestUnit(marines[0]->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER);
			Point2D recallPoint = bases[bases.size()-1]->origin;
		
			actions->UnitCommand(marines, ABILITY_ID::SMART, recallPoint);
			actions->UnitCommand(reapers, ABILITY_ID::SMART, recallPoint);
			actions->UnitCommand(marauder, ABILITY_ID::SMART, recallPoint);
			actions->UnitCommand(tank, ABILITY_ID::SMART, recallPoint);
			actions->UnitCommand(hellion, ABILITY_ID::SMART, recallPoint);
		}
	}
	


	return false;
}

bool CombatManager::AllOutAttackEnemy()
{
	Units army, next_army_batch, enemies = observation->GetUnits(Unit::Alliance::Enemy);
	float closest_d = std::numeric_limits<float>::max();
	Point2D target_point = enemyStartLocation;
	

	if (!foundEnemyBase) {
		return false;
	}

	// Get all army units
	for (auto unit_type : BSB_TerranUnitCategories::MAIN_ARMY_UNITS()) {
		next_army_batch = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
		army.insert(army.end(), next_army_batch.begin(), next_army_batch.end());
	};

	// Do nothing if there's no army to command
	if (army.size() == 0) {
		return false;
	}

	// If an all-out attack wasn't already called, send entire army to enemy's main base
	if (!allOutAttack) {
		actions->UnitCommand(army, ABILITY_ID::ATTACK_ATTACK, enemyStartLocation);
		lastAllOutPos = enemyStartLocation;
		allOutAttack = true;
	}
	// If all-out attack in progress, keep targeting enemies close to enemy base until all are wiped out.
	else {
		// Do nothing if there's no enemies in sight
		if (enemies.size() == 0) {
			return false;
		}

		for (auto e : enemies) {
			float d = DistanceSquared2D(e->pos, lastAllOutPos);
			if (d < closest_d) {
				closest_d = d;
				target_point = e->pos;
			}
		}
		// Goto nearest enemy if its some distance from the last attack location
		if (closest_d < 30.0f) {
			lastAllOutPos = target_point;
			actions->UnitCommand(army, ABILITY_ID::ATTACK_ATTACK, lastAllOutPos);
		}
	}

	return true;
}

void CombatManager::OnIdleMarine(const Unit* unit) {
	const GameInfo& game_info = observation->GetGameInfo();
	Point2D newPoint = bases[bases.size()-1]->origin;
	size_t randomMarineLocation = 0;
	size_t numberMarines = CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();
	//Send every #th idle marine to a random enemy location so that we can spot enemy
	if ((numberIdleMarines % 70) == 0) {
		randomMarineLocation = rand() % expansionLocations.size();
		newPoint = expansionLocations[randomMarineLocation];
		numberIdleMarines = 1;
	}//Send others close to base
	else {
		newPoint = Point2D(newPoint.x + rx * 15.0f, newPoint.y + ry * 15.0f);
	}
	//Only send marines out if we have a small army
	if (numberMarines >= 40) {
		actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
	}
	numberIdleMarines += 1;
}

void CombatManager::OnIdleReaper(const Unit* unit) {

}

bool CombatManager::FindEnemyBase()
{
	static int closeEnemiesPushover;
	static int printer;
	const GameInfo& game_info = observation->GetGameInfo();
	Point2D p;
	if (enemyStartLocation != p) {
		for (auto beg = begin(scoutingMarines); beg != end(scoutingMarines); ++beg) {
			Point2D newPoint = bases[bases.size() - 1]->origin;
			newPoint = Point2D(newPoint.x + GetRandomScalar() * 15.0f, newPoint.y + GetRandomScalar() * 15.0f);
			actions->UnitCommand(beg->first, ABILITY_ID::ATTACK_ATTACK, newPoint);
		}
		scoutingMarines.clear();
		return true;
	}
	//    for (auto unit : units) {

	//If there are less marines currently scouting than there are possible enemy locations.
	//Send more marines to scout.
	if (scoutingMarines.size() < game_info.enemy_start_locations.size()) {
		Units marines = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_MARINE));
		int round;
		for (auto marine : marines) {
			//make sure there still aren't enough scouters
			if (scoutingMarines.size() >= game_info.enemy_start_locations.size()) {	}

			//make sure that this marine is not already scouting
			else if (scoutingMarines.find(marine) != scoutingMarines.end()) {  }

			//add this marine to the scouters and send them off to attack
			else {
				Point2D possibleLocation = game_info.enemy_start_locations[scoutingMarines.size()];
				scoutingMarines[marine] = possibleLocation;
				actions->UnitCommand(marine, ABILITY_ID::ATTACK_ATTACK, possibleLocation);
			}

		}
		
	}

	for (auto beg = begin(scoutingMarines); beg != end(scoutingMarines); ++beg) {
		actions->UnitCommand(beg->first, ABILITY_ID::ATTACK_ATTACK, scoutingMarines[beg->first]);
		if ((beg->first->health_max - beg->first->health) > 0) {
			Units enemies = observation->GetUnits(Unit::Alliance::Enemy);
			int closeEnemies = 0;
			for (auto enemy : enemies) {
				if (closeEnemies > 5) { break; }
				if (Distance3D(enemy->pos, beg->first->pos) < 15) { ++closeEnemies; }
			}
			if (closeEnemies > 5) {
				enemyStartLocation = beg->second;
				std::cout << "Found enemy base at (" << enemyStartLocation.x << "," << enemyStartLocation.y << ")" << std::endl;
				return true;
			}
			
		}
	}

	return false;
}
