#include "CombatManager.h"

CombatManager::CombatManager()
{
	InitArmyCounts();
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

	if (observation->GetArmyCount() > 40) {
		attack = true;
	}

	if (attack) {
		//Send all units to the enemy
		Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
		Units sending = observation->GetUnits(Unit::Alliance::Self, filter);
		for (auto s : sending) {
			actions->UnitCommand(s, ABILITY_ID::ATTACK_ATTACK, target);
		}
		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		Units moreSending = observation->GetUnits(Unit::Alliance::Self, filter);
		for (auto s : moreSending) {
			actions->UnitCommand(s, ABILITY_ID::ATTACK_ATTACK, target);
		}
		//Actions()->UnitCommand(moreSending, ABILITY_ID::ATTACK_ATTACK, target);

		//Reset idle marine count
		numberIdleMarines = 0;

	}

	return false;
}

void CombatManager::InitArmyCounts()
{
	for (auto type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
		army_counts[type] = 0;
	}
}

void CombatManager::UpdateArmyCounts() 
{
	for (auto type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
		army_counts[type] = observation->GetUnits(Unit::Alliance::Self, IsUnit(type)).size();
	}
}

size_t CombatManager::GetArmySize() {
	size_t army_size = 0;

	for (auto& it : army_counts) {
		army_size += it.second;
	}

	return army_size;
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
	}//Send others close to base
	else {
		return OnIdleSmart(unit, UNIT_TYPEID::TERRAN_MARINE);
	}
	//Only send marines out if we have a small army
	if (numberMarines > 30) {
		Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
	}
	numberIdleMarines += 1;
}

void CombatManager::OnIdleSmart(const Unit* unit, UNIT_TYPEID unit_type) {
	Point2D start = observation->GetStartLocation();
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();

	// If this unit is far from home base, send it back
	if (DistanceSquared2D(start, unit->pos) > 30.0f) {
		actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, Point2D(start.x + rx * 5.0f, start.y + ry * 5.0f));
	}
}