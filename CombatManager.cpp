#include "CombatManager.h"

CombatManager::CombatManager()
{

}

void CombatManager::OnIdleMarine(const Unit* unit) {
	const GameInfo& game_info = observation->GetGameInfo();
	Point2D newPoint = observation->GetStartLocation();
	size_t numberMarines = CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
	float rx = GetRandomScalar();
	float ry = GetRandomScalar();
	//Send every 10th idle marine to a random enemy location so that we can spot enemy
	/*if ((numberMarines % 20) == 0) {
		int randomMarineLocation = rand() % game_info.enemy_start_locations.size();
		newPoint = game_info.enemy_start_locations[randomMarineLocation];
	}*/
	if (scoutingMarines.size() < game_info.enemy_start_locations.size()) {
		newPoint = game_info.enemy_start_locations[scoutingMarines.size()];
		scoutingMarines[unit] = newPoint;
		actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
	}
	//Send others close to base
	else {
		newPoint = Point2D(newPoint.x + rx * 15.0f, newPoint.y + ry * 15.0f);
	}
	//Only send marines out if we have a small army
	if (numberMarines > 20) {
		actions->UnitCommand(unit, ABILITY_ID::SMART, newPoint);
	}
}

void CombatManager::OnIdleReaper(const Unit* unit) {

}

void CombatManager::CheckForEnemyBase()
{	
	Point2D testPoint;
	if (enemyStartLocation != testPoint) {
		return;
	}

	if (scoutingMarines.size() == 0) {
		return;
	}

	for (auto beg = begin(scoutingMarines); beg != end(scoutingMarines); ++beg) {
		if ( ( (beg->first)->health_max - (beg->first)->health ) > 0) {
			enemyStartLocation = beg->second;
			std::cout << "Found enemy base at (" << enemyStartLocation.x << "," << enemyStartLocation.y << ")" << std::endl;
			return;
		}
	}

	return;
}


