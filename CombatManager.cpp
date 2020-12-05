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
	filter = IsUnit(UNIT_TYPEID::TERRAN_HELLIONTANK);
	Units hellbat = observation->GetUnits(Unit::Alliance::Self, filter);

	if (marines.size() > 1) {
		if (attack && (target != nullptr)) {
			//TODO: Allow other units to attack
			//We have already spotted an enemy
			actions->UnitCommand(marines, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(reapers, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(marauder, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(tank, ABILITY_ID::ATTACK_ATTACK, target);
			HellionMorph(hellion, attack, target->pos);
			return true;
		} else if ((observation->GetArmyCount() > 80) && (target != nullptr)) {
			//TODO: Allow other units to attack
			//We have already spotted an enemy
			actions->UnitCommand(marines, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(reapers, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(marauder, ABILITY_ID::ATTACK_ATTACK, target);
			actions->UnitCommand(tank, ABILITY_ID::ATTACK_ATTACK, target);
			HellionMorph(hellion, attack, target->pos);
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
				//If almost to enemy, transform
				float d = DistanceSquared2D(hellion[0]->pos, attackPoint);
				if (d < 30) {
					for (const auto& h : hellion) {
						actions->UnitCommand(h, ABILITY_ID::MORPH_HELLBAT);
						actions->UnitCommand(h, ABILITY_ID::ATTACK, attackPoint, true);
					}
				}
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
			HellionMorph(hellbat, false, recallPoint);
		}
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

	actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, halfway);
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
	actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, halfway);
}

void CombatManager::OnIdleMarauder(const Unit* unit) {
	actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, halfway);
}

void CombatManager::OnIdleHellion(const Unit* unit) {
	if (unit->unit_type == UNIT_TYPEID::TERRAN_HELLIONTANK && CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) != 0) {
		actions->UnitCommand(unit, ABILITY_ID::MORPH_HELLION);
	}
	actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, halfway);
}

void CombatManager::OnIdleSeigeTank(const Unit* unit) {
	if (unit->unit_type == UNIT_TYPEID::TERRAN_SIEGETANKSIEGED) {
		actions->UnitCommand(unit, ABILITY_ID::MORPH_UNSIEGE);
	}
	actions->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, halfway);
}

//Ultilities for morphing units
void CombatManager::HellionMorph(const Units units, bool attacking, Point2D to) {
	if (!units.empty()) {
		float distanceAway = Distance2D(units.back()->pos, to);
		for (const auto& hellion : units) {
			// if attacking, transform to Hellbat and attack target
			if (attacking) {
				if (distanceAway < 30 && (CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) != 0)) {
					actions->UnitCommand(hellion, ABILITY_ID::MORPH_HELLBAT);
				}
				actions->UnitCommand(hellion, ABILITY_ID::ATTACK_ATTACK, to);
			}
			// if retreating, transform to Hellion and move to location
			else {
				actions->UnitCommand(hellion, ABILITY_ID::MORPH_HELLION);
				actions->UnitCommand(hellion, ABILITY_ID::SMART, to);
			}
		}
	}
}

void CombatManager::SiegeTankMorph(const Units units, bool attacking, Point2D to) {
	if (!units.empty()) {
		float distanceAway = Distance2D(units.front()->pos, to);
		for (const auto& siegeTank : units) {
			// if attacking, transform to Hellbat and attack target
			if (attacking && (CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) != 0)) {
				if (distanceAway < 30) {
					actions->UnitCommand(siegeTank, ABILITY_ID::MORPH_SIEGEMODE);
				}
				actions->UnitCommand(siegeTank, ABILITY_ID::ATTACK_ATTACK, to);
			}
			// if retreating, transform to Hellion and move to location
			else {
				actions->UnitCommand(siegeTank, ABILITY_ID::MORPH_UNSIEGE);
				actions->UnitCommand(siegeTank, ABILITY_ID::SMART, to);
			}
		}
	}
}