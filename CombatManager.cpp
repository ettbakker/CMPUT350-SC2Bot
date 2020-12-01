#include "CombatManager.h"

CombatManager::CombatManager()
{

}

bool CombatManager::AttackEnemy() {
	Units enemies = observation->GetUnits(Unit::Alliance::Enemy);
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
			if (DistanceSquared2D(e->pos, b->origin) < 100) {
				attack = true;
			}
		}
	}

	if (observation->GetArmyCount() > 50) {
		attack = true;
	}

	if (attack) {
		//Send all units to the enemy
		Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
		Units marines = observation->GetUnits(Unit::Alliance::Self, filter);
		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		Units reapers = observation->GetUnits(Unit::Alliance::Self, filter);
		if (target != nullptr) {
			actions->UnitCommand(marines, ABILITY_ID::ATTACK_ATTACK, target, true);
		}
		if (target != nullptr) {
			actions->UnitCommand(reapers, ABILITY_ID::ATTACK_ATTACK, target, true);
		}
		//Reset idle marine count
		numberIdleMarines = 0;

	}

	return false;
}

void CombatManager::OnIdleMarine(const Unit* unit) {
	const GameInfo& game_info = observation->GetGameInfo();
	Point2D newPoint = observation->GetStartLocation();
	size_t randomMarineLocation = 0;
	size_t numberMarines = CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();
	//Send every #th idle marine to a random enemy location so that we can spot enemy
	if ((numberIdleMarines % 30) == 0) {
		randomMarineLocation = rand() % game_info.enemy_start_locations.size();
		newPoint = game_info.enemy_start_locations[randomMarineLocation];
		numberIdleMarines = 0;
	}//Send others close to base
	else {
		newPoint = Point2D(newPoint.x + rx * 5.0f, newPoint.y + ry * 5.0f);
	}
	//Only send marines out if we have a small army
	if (numberMarines > 30) {
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
	}
	numberIdleMarines += 1;
}

void CombatManager::OnIdleReaper(const Unit* unit) {

}
