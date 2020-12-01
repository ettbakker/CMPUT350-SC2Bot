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

	if (observation->GetArmyCount() > 10) {
		attack = true;
	}

	if (attack) {
		//Send all units to the enemy
		Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
		Units marines = observation->GetUnits(Unit::Alliance::Self, filter);
		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		Units reapers = observation->GetUnits(Unit::Alliance::Self, filter);
		bool stillMarines = true, stillReapers = true;
		auto mit = marines.begin();
		auto rit = reapers.begin();
		//Loop through large vecotor and give small vectors a command until there are no
		//more units to add to small vectors.
		while (stillMarines || stillReapers) {
			//Units *tempMarines;
			if (marines.end() - mit > 30) {
				std::cout << "Send 30" << std::endl;
				Units tempMarines(mit, mit + 30);
				actions->UnitCommand(tempMarines, ABILITY_ID::ATTACK_ATTACK, attack);
				mit += 30;
			}
			else if( mit < marines.end()){
				std::cout << "Send left overs" << std::endl;
				Units tempMarines(mit, marines.end()-1);
				actions->UnitCommand(tempMarines, ABILITY_ID::ATTACK_ATTACK, attack);
				stillMarines = false;
				mit = marines.end();
			}
			else {
				std::cout << "none left" << std::endl;
				stillMarines = false;
			}
			//Units* tempReapers;
			if (reapers.end() -rit > 30) {
				Units tempReapers(rit, rit + 30);
				rit += 30;
				actions->UnitCommand(tempReapers, ABILITY_ID::ATTACK_ATTACK, target);
			}
			else if (rit < reapers.end()){
				Units tempReapers(rit, reapers.end()-1);
				stillReapers = false;
				actions->UnitCommand(tempReapers, ABILITY_ID::ATTACK_ATTACK, target);
				rit = reapers.end();
			}
			else {
				stillReapers = false;
			}
			
			
		}

		/*
		//Send all units to the enemy
		Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
		Units sending = observation->GetUnits(Unit::Alliance::Self, filter);
		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		Units moreSending = observation->GetUnits(Unit::Alliance::Self, filter);
		for (auto s : sending) {
			actions->UnitCommand(s, ABILITY_ID::ATTACK_ATTACK, target);
		}
		//Actions()->UnitCommand(sending, ABILITY_ID::ATTACK_ATTACK, target);
		
		for (auto s : moreSending) {
			actions->UnitCommand(s, ABILITY_ID::ATTACK_ATTACK, target);
		}
		//Actions()->UnitCommand(moreSending, ABILITY_ID::ATTACK_ATTACK, target);

		//Reset idle marine count
		numberIdleMarines = 0;*/

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
