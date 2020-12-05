#include "CombatManager.h"

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


	if (target != nullptr) {
		for (auto armyUnit : TerranUnitCategories::BARRACKS_UNITS()) {
			Filter filter = IsUnit(armyUnit);
			Units units = observation->GetUnits(Unit::Alliance::Self, filter);
			if (units.size() > 0) {
				if (attack) {
					//We have already spotted an enemy
					actions->UnitCommand(units, ABILITY_ID::ATTACK, target);
				}
				else if ((observation->GetArmyCount() > 80)) {
					actions->UnitCommand(units, ABILITY_ID::ATTACK, target);
				}

				if (observation->GetArmyCount() < 30) {
					//If there is a target and they are far away, call back all units so they don't keep chasing it
					//because our army is still small
					//Recall to nearest command center
					Point2D recallPoint = bases[bases.size() - 1]->origin;
					actions->UnitCommand(units, ABILITY_ID::SMART, recallPoint);
				}
			}
		}
	}
	else if (observation->GetArmyCount() > 60)  {

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
		float rx;
		float ry;
		if (CountUnitType(UNIT_TYPEID::TERRAN_HELLION) > 0) {
			Filter filter = IsUnit(UNIT_TYPEID::TERRAN_HELLION);
			Units hellion = observation->GetUnits(Unit::Alliance::Self, filter);
			for (size_t i = 0; (i < hellion.size())|| (i<5); i++) {
				//Send the units randomly around the location
				rx = GetRandomScalar();
				ry = GetRandomScalar();
				attackPoint = Point2D(attackPoint.x + rx * 15.0f, attackPoint.y + ry * 15.0f);
				actions->UnitCommand(hellion[0], ABILITY_ID::ATTACK, attackPoint, true);
			}
		}
		else if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) >0) {
			Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
			Units marines = observation->GetUnits(Unit::Alliance::Self, filter);
			for (size_t i = 0; (i < marines.size()) || (i < 5); i++) {
				//Send the units randomly around the location
				rx = GetRandomScalar();
				ry = GetRandomScalar();
				attackPoint = Point2D(attackPoint.x + rx * 15.0f, attackPoint.y + ry * 15.0f);
				actions->UnitCommand(marines[0], ABILITY_ID::ATTACK, attackPoint, true);
			}
		}

		return true;
	}


	


	return false;
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
