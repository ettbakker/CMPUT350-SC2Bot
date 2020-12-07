#include "CombatManager.h"

CombatManager::CombatManager()
{

}

bool CombatManager::AttackEnemy() {
	Units enemies = observation->GetUnits(Unit::Alliance::Enemy);
	Units hellbats = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_HELLIONTANK));
	const GameInfo& game_info = observation->GetGameInfo();
	float distance = std::numeric_limits<float>::max();
	Units army, next_army_batch;

	//run the for loop here, but if all out attack then to save computation, we check for
	//enemmies near base in the allOutAttack loop.
	if (!allOutAttack) {
		targetAtBase = nullptr;
		defendBase = false;
		//For all enemies, defendBase the one that is closest to the base
		for (auto e : enemies) {
			for (auto b : bases) {
				float d = DistanceSquared2D(e->pos, b->origin);
				if (d < distance)
				{
					distance = d;
					targetAtBase = e;
				}
				if (d < 1000) {
					defendBase = true;
				}
			}
		}
	}
	
	if (targetAtBase != nullptr) {
		// Get all army units
		for (auto unit_type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
			next_army_batch = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
			army.insert(army.end(), next_army_batch.begin(), next_army_batch.end());
			//std::cout << "ARMY SIZE:" << army.size() << "\n";
		};

		//If doing all out attack still check for enemies near the base
		if (allOutAttack && defendBase) {
			//Send a small squad to attack it
			if (((CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > 10) && numEnemyAtBase < 2)) {
				Filter filter = IsUnit(UNIT_TYPEID::TERRAN_MARINE);
				Units marines = observation->GetUnits(Unit::Alliance::Self, filter);
				for (size_t i = 0; (i < marines.size()); i++) {
					if (i == 15) { //Up to 15 marines
						break;
					}
					if (targetAtBase != nullptr) {
						actions->UnitCommand(marines[i], ABILITY_ID::ATTACK, targetAtBase);
					}
				}
				numEnemyAtBase = 0;
			}//Send all army units to defend the base because they are overwhelming us
			else {
				if (targetAtBase != nullptr) {
					HellionMorph(hellbats, false, targetAtBase->pos);
					actions->UnitCommand(army, ABILITY_ID::ATTACK, targetAtBase);
				}
			}
		}
		else if (!allOutAttack) {
			if (army.size() > 0) {
				if (defendBase) {
					//We have already spotted an enemy
					actions->UnitCommand(army, ABILITY_ID::ATTACK, targetAtBase);
				}
				else if ((army.size() > 130)) {
					actions->UnitCommand(army, ABILITY_ID::ATTACK, targetAtBase);
				}
				else {
					//If there is a targetAtBase and they are far away, call back all units so they don't keep chasing it
					//because our army is still small
					//Recall to nearest command center
					Point2D recallPoint = bases[bases.size() - 1]->origin;
					actions->UnitCommand(army, ABILITY_ID::SMART, recallPoint);
				}
			}
			
		}
	}
	

	return false;
}

bool CombatManager::AllOutAttackEnemy()
{
	Units army, next_army_batch, enemies = observation->GetUnits(Unit::Alliance::Enemy);
	float closest_d = std::numeric_limits<float>::max();
	float closest_dtoBase = std::numeric_limits<float>::max();
	Point2D target_point = lastAllOutPos;
	bool newTarget = false;
	targetAtBase = nullptr;
	

	if (!foundEnemyBase) {
		return false;
	}

	// Get all army units
	for (auto unit_type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
		next_army_batch = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
		army.insert(army.end(), next_army_batch.begin(), next_army_batch.end());
		//std::cout << "ARMY SIZE:" << army.size() << "\n";
	};

	// Do nothing if there's no army to command
	// Also cancels the allOutAttack if we failed.
	if (army.size() <= 10) {
		allOutAttack = false;
		return false;
	}

	// If an all-out attack wasn't already called, send entire army to enemy's main base
	// Wait until we have an acceptable amount of units. This is important if an attack fails and the got recalled.
	if (!allOutAttack && (army.size() > 30)) {
		lastAllOutPos = enemyStartLocation;
		allOutAttack = true;
		defendBase = false;
		Point2D p;
		if (enemyStartLocation != p) {
			sortAndAddSweepLocations(enemyStartLocation);
		}
		else { //Just start the sweep from a possible enemy location if we still haven't found it.
			sortAndAddSweepLocations(observation->GetGameInfo().enemy_start_locations[0]);
		}
		actions->UnitCommand(army, ABILITY_ID::ATTACK, enemyStartLocation);
		std::cout << "All Out Attack " << std::endl;
	}
	// If all-out attack in progress, keep targeting enemies close to enemy base until all are wiped out.
	else {
		// Do nothing if there's no enemies in sight
		// If we aren't finding any new enemies after a while, just start sweeping
		if ((enemies.size() == 0) || (numberTimesSinceNewTarget >= 300)) {
			return updateSweeping(army, enemies);
		}

		defendBase = false;
		numEnemyAtBase = 0;
		for (auto e : enemies) {
			//Check for targets near the base at the same time so that attackEnemy can take care of them
			float dtoB = DistanceSquared2D(e->pos, bases[0]->origin);
			if (dtoB < 500) {
				++numEnemyAtBase;
				if ((dtoB < closest_dtoBase) && (dtoB < 500)) {
					closest_dtoBase = dtoB;
					targetAtBase = e;
					defendBase = true;
				}
			}

			//Check for new targets based on the last attack point
			float d = DistanceSquared2D(e->pos, lastAllOutPos);
			if (d < closest_d) {
				closest_d = d;
				target_point = e->pos;

			}
		}

		//If multiple enemies are at base, start attacking it.
		//Fixes bug where they run back and forth.
		if (numEnemyAtBase > 6) {
			target_point = targetAtBase->pos;
		}


		// Update attack pos if nearest enemy is some distance from the last attack location
		if (closest_d > 15.0f) {
			lastAllOutPos = target_point;
			newTarget = true;
		}
		//Only attack the target if its a new one or there's more than one. Prevents useless running around
		//while sweeping.
		if (newTarget) {
			numberTimesSinceNewTarget = 0;
			actions->UnitCommand(army, ABILITY_ID::ATTACK_ATTACK, target_point);
		}
		else {
			if ((++numberTimesSinceNewTarget % 25) == 0) {
				//std::cout << "num " << numberTimesSinceNewTarget << std::endl;
				lastAllOutPos = GetRandomNearbyPoint(lastAllOutPos, 8.0);
			}
			actions->UnitCommand(army, ABILITY_ID::ATTACK_ATTACK, lastAllOutPos);
		}

	}

	return true;
}

//Sweeping lets the army go from location to location to find other enemy bases.
//The sweeping locations are sorted from closest to enemy base which was determined at the start of the base
//If too many locations have been sweeped with no enemies, we start sending reapers back to random locations since we
//most likely missed the last few buildings.
bool CombatManager::updateSweeping(Units &army, Units &enemies) {
	Units next_army_batch;
	//If we are near the sweep location start moving to next sweep location
	if ((sweepLocations.size() > 0) && (!defendBase)) {
		//lastAllOutPos = sweepLocations[sweepLocationCounter];
		if (sweepLocationCounter == 0) {
			lastAllOutPos = sweepLocations[sweepLocationCounter++];
			actions->UnitCommand(army, ABILITY_ID::ATTACK, lastAllOutPos);
			return true;
			//std::cout << "Sweeping" << std::endl;
		}
		else {	//Only update the sweeping location if at least 20 units have reached the sweeping location
			if (numberTimesSinceNewTarget >= 300) {
				size_t numReached = 0;
				for (size_t i = 0; i < army.size(); i++) {
					if (numReached == 20) {
						break;
					}
					else if (Distance2D(army[i]->pos, lastAllOutPos) < 7.0) {
						++numReached;
					}
				}
				if (numReached >= 20) {
					lastAllOutPos = sweepLocations[sweepLocationCounter++];
					std::cout << "Sweep Next Location. " << "Counter: " << sweepLocationCounter << std::endl;
					numberTimesSinceNewTarget = 0;
					actions->UnitCommand(army, ABILITY_ID::ATTACK, lastAllOutPos);
					return false;
				}
			}
			else {
				//Play around the location a bit before moving on to the next sweeping location
				//Only does this if it still hasn't spotted an enemy
				if ((++numberTimesSinceNewTarget % 25) == 0) {
					lastAllOutPos = GetRandomNearbyPoint(sweepLocations[sweepLocationCounter - 1], 10.0);
				}
			}
		}
		if (!observation->IsPathable(lastAllOutPos)) {
			lastAllOutPos = sweepLocations[sweepLocationCounter - 1];
		}

		if ((numberTimesSinceNewTarget % 25) == 0) {
			//If we've swepped 4 locations and still haven't found anything start sending reapers back to random points
			if (sweepLocationCounter >= 4) {
				Units reapers = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REAPER));
				for (size_t i = 0; i < reapers.size(); i++) {
					//Send each out to a random point
					actions->UnitCommand(reapers[i], ABILITY_ID::ATTACK,
						GetRandomNearbyPoint(sweepLocations[rand() % sweepLocationCounter], 15.0), true);
				}
				//Rebuild the army without reapers
				army.clear();
				for (auto unit_type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
					if (unit_type != UNIT_TYPEID::TERRAN_REAPER) {
						next_army_batch = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
						army.insert(army.end(), next_army_batch.begin(), next_army_batch.end());
					}
				}
			}
			actions->UnitCommand(army, ABILITY_ID::ATTACK, lastAllOutPos);
			return true;
		}
	}
	else if (enemies.size() == 0) {
		defendBase = false;
	}
	return false;
}

void CombatManager::CalculateGatherLocation() {
	//Accumulate the army somewhere near the enemy base. Helps with attacking at the same time.
	Point2D p;
	if (enemyStartLocation != p) {

		//Determine whereabouts of enemie and make gather location in that direction
		if ((GetStartPoint().x < enemyStartLocation.x) || (GetStartPoint().y < enemyStartLocation.y)) {
			gatherLocation.x = (GetStartPoint().x + enemyStartLocation.x) / 2;
			gatherLocation.y = (GetStartPoint().y + enemyStartLocation.y) / 2;
		}
		else if ((GetStartPoint().x > enemyStartLocation.x) || (GetStartPoint().y > enemyStartLocation.y)) {
			gatherLocation.x = (GetStartPoint().x - enemyStartLocation.x) / 2;
			gatherLocation.y = (GetStartPoint().y - enemyStartLocation.y) / 2;
		}

		//Get a placeable and pathable location for gather location
		Point2D tempPoint = gatherLocation;
		for (size_t i = 0; i < 30; i++) {
			if ((observation->IsPathable(tempPoint)) && (observation->IsPathable(tempPoint))) {
				gatherLocation = tempPoint;
				std::cout << "Gather at X:" << gatherLocation.x << " Y:" << gatherLocation.y << std::endl;
				return;
			}
			tempPoint = GetRandomNearbyPoint(gatherLocation, 5.0);
		}
	}
	gatherLocation = FindCenterOfMap(observation->GetGameInfo());
	std::cout << "Gather at X:" << gatherLocation.x << " Y:" << gatherLocation.y << std::endl;
}

void CombatManager::GatherNearEnemy() {
	Units army, next_army_batch;
	// Get all army units
	for (auto unit_type : TerranUnitCategories::ALL_COMBAT_UNITS()) {
		next_army_batch = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
		army.insert(army.end(), next_army_batch.begin(), next_army_batch.end());
		//std::cout << "ARMY SIZE:" << army.size() << "\n";
	};

	//Send them to a location near the enemy base
	actions->UnitCommand(army, ABILITY_ID::SMART, gatherLocation);

}


void CombatManager::OnIdleMarine(const Unit* unit) {
	if (allOutAttack) {
		return;
	}
	const GameInfo& game_info = observation->GetGameInfo();
	Point2D newPoint = bases[bases.size()-1]->origin;
	size_t randomMarineLocation = 0;
	size_t numberMarines = CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
	//Send every #th idle marine to a random enemy location so that we can spot enemy
	if (allOutAttack || ((numberIdleMarines % 50) == 0)) {
		//Went through all sweep location so fail safe is to just keep searching
		if (sweepLocationCounter >= sweepLocations.size()) {
			size_t randomLoc = 0;
			if ((rand() % 2) == 0) {
				randomLoc = rand() % game_info.enemy_start_locations.size();
				newPoint = game_info.enemy_start_locations[randomLoc];
			}
			else {
				randomLoc = rand() % expansionLocations.size();
				newPoint = expansionLocations[randomLoc];
			}
		}
		else {
			//Get nearby points to current sweeping location
			newPoint = sweepLocations[sweepLocationCounter];
		}
		newPoint = GetRandomNearbyPoint(newPoint, 15.0);
		numberIdleMarines = 0;
	}//Send others close to base
	else if(!allOutAttack) {
		newPoint = GetRandomNearbyPoint(newPoint, 5.0);
	}
	//Only send marines out if we have a small army
	if ((numberMarines >= 30) || allOutAttack) {
		actions->UnitCommand(unit, ABILITY_ID::SMART, newPoint);
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
			newPoint = GetRandomNearbyPoint(newPoint, 15.0);
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
				//enemyStartLocation = game_info.enemy_start_locations[0];
				std::cout << "Found enemy base at (" << enemyStartLocation.x << "," << enemyStartLocation.y << ")" << std::endl;
				foundEnemyBase = true;
				return true;
			}
			
		}

		if (!beg->first->is_alive) {
			enemyStartLocation = beg->second;
			//enemyStartLocation = game_info.enemy_start_locations[0];
			std::cout << "Found enemy base at (" << enemyStartLocation.x << "," << enemyStartLocation.y << ")" << std::endl;
			foundEnemyBase = true;
			return true;
		}
	}

	return false;
}


//Sort sweeping locations from closest to enemy base location so when we run out of enemies our army will start sweeping.
void CombatManager::sortAndAddSweepLocations(Point2D fromPoint) {
	Point2D tempPoint;
	Point2D point1;
	Point2D point2;
	int distance1;
	int distance2;
	bool stillSwapping = false;
	for (size_t i = 0; i < expansionLocations.size(); i++) {
		sweepLocations.push_back(expansionLocations[i]);
	}
	for (size_t i = 0; i < observation->GetGameInfo().enemy_start_locations.size(); i++) {
		sweepLocations.push_back(observation->GetGameInfo().enemy_start_locations[i]);
	}
	for (size_t i = 0; i < sweepLocations.size(); i++) {
		for (size_t j = 0; j < sweepLocations.size() - i - 1; j++) {
			point1 = sweepLocations[j];
			point2 = sweepLocations[j+1];
			distance1 = Distance2D(fromPoint, point1);
			distance2 = Distance2D(fromPoint, point2);
			if (distance1 > distance2) {
				//Swap them
				tempPoint = sweepLocations[j];
				sweepLocations[j] = sweepLocations[j + 1];
				sweepLocations[j + 1] = tempPoint;
				stillSwapping = true;
			}
		}
		if (!stillSwapping) {
			break;
		}
	}
}

void CombatManager::HellionMorph(const Units units, bool attacking, Point2D to) {
	if (!units.empty()) {
		float distanceAway = Distance2D(units.back()->pos, to);
		for (const auto& hellion : units) {
			// if attacking, transform to Hellbat and attack target
			if (attacking) {
				if (distanceAway < 65 && (CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) != 0)) {
					actions->UnitCommand(hellion, ABILITY_ID::MORPH_HELLBAT);
				}
				//actions->UnitCommand(hellion, ABILITY_ID::ATTACK_ATTACK, to);
			}
			// if retreating, transform to Hellion and move to location
			else {
				actions->UnitCommand(hellion, ABILITY_ID::MORPH_HELLION);
				//actions->UnitCommand(hellion, ABILITY_ID::SMART, to);
			}
		}
	}
}

