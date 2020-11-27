#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() 
{ 
	std::cout << "hello, World!" << std::endl;
	bases.push_back(Observation()->GetStartLocation());
	return; 
}

//Print the results of the players once the game is over.
void BasicSc2Bot::OnGameOver() {
	std::vector<PlayerResult> res = Observation()->GetResults();
	for(auto r : res){
		std::cout << r.player_id << r.result << std::endl;
	}
	std::cout << "Game Over" << std::endl;
}

void BasicSc2Bot::OnStep() 
{ 
	
	botFunctions->setVars(Observation(), Actions());
	builder->setVars(Observation(), Actions(), bases);
	builder->BuildStructures();
	ManageSCV();
	AttackEnemy();
	//resetBasesNumber();
	//fixBuildings();
	return; 
}

void BasicSc2Bot::OnUnitIdle(const Unit* unit)
{
	switch (unit->unit_type.ToType())
	{
		case UNIT_TYPEID::TERRAN_COMMANDCENTER:
		{
			if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_SCV) < 30*bases.size()) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
			}
			break;
		}
		

		case UNIT_TYPEID::TERRAN_SCV: 
		{
			GiveSCVACommand(unit);
			
			break;
		}

		case UNIT_TYPEID::TERRAN_BARRACKS: 
		{
			if (botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_REAPER)) {
				Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER);
			}
			Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
			break;
		}

		case UNIT_TYPEID::TERRAN_MARINE: 
		{
			const ObservationInterface* observation = Observation();
			const GameInfo& game_info = observation->GetGameInfo();
			Point2D newPoint = observation->GetStartLocation();
			size_t randomMarineLocation = 0;
			size_t numberMarines = botFunctions->CountUnitType(UNIT_TYPEID::TERRAN_MARINE);
			float rx = GetRandomScalar();
			float ry = GetRandomScalar();
			//Send every #th idle marine to a random enemy location so that we can spot enemy
			if ((numberIdleMarines % 30) == 0) {
				randomMarineLocation = rand() % game_info.enemy_start_locations.size();
				newPoint = game_info.enemy_start_locations[randomMarineLocation];
			}//Send others close to base
			else {
				newPoint = Point2D(newPoint.x + rx * 5.0f, newPoint.y + ry * 5.0f);
			}
			//Only send marines out if we have a small army
			if (numberMarines > 30) {
				Actions()->UnitCommand(unit, ABILITY_ID::ATTACK_ATTACK, newPoint);
			}
			numberIdleMarines += 1;
			break;
		}
		case UNIT_TYPEID::TERRAN_REAPER:
		{
			
			break;
		}

		case UNIT_TYPEID::TERRAN_ARMORY:
		{
			std::vector<UpgradeID> upgrades = Observation()->GetUpgrades();
			
			
			for (auto u : upgrades) {
				if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL3) {
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3);
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3);
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3);
					break;
				}
				if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL2) {
					std::cout << "Reasearching" << std::endl;
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2);
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2);
					Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2);
					break;
				}
			}
			
			Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1);
			Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1);
			Actions()->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1);
			break;
		}


		default:
		{
			break;
		}
	}
}

//private

bool BasicSc2Bot::AttackEnemy() {
	const ObservationInterface* observation = Observation();
	Units enemies = observation->GetUnits(Unit::Alliance::Enemy);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	bool attack = false;
	//For all enemies, attack the one that is closest to the base
	for(auto e: enemies){
		for (auto b : bases) {
			float d = DistanceSquared2D(e->pos, b);
			if (d < distance)
			{
				distance = d;
				target = e;
			}
			if (DistanceSquared2D(e->pos, b) < 100) {
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
		filter = IsUnit(UNIT_TYPEID::TERRAN_REAPER);
		Units moreSending = observation->GetUnits(Unit::Alliance::Self, filter);
		sending.insert(sending.end(), moreSending.begin(), moreSending.end());
		
		Actions()->UnitCommand(sending, ABILITY_ID::ATTACK_ATTACK, target);
		//Reset idle marine count
		numberIdleMarines = 0; 
		
	}

	return false;
}



bool BasicSc2Bot::fixBuildings(){
		
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits();
	
	for (auto u : units) {
		if (u->health < u->health_max) {
			//std::cout << "Building Damaged" << std::endl;
			// Also get an scv to build the structure.
			const Unit* unit_to_build = nullptr;
			Units scvUnit = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
			for (const auto& unit : scvUnit)
			{
				for (const auto& order : unit->orders)
				{
					if (order.ability_id == ABILITY_ID::SMART)
					{
						return false;
					}
				}
				unit_to_build = unit;
				
			}
			
			
			Actions()->UnitCommand(unit_to_build, ABILITY_ID::SMART, u);

		}
	}
	


	return false;
}



//For any building that has harvesters, if there are too many harvesters
//and there is more that one of that building, then stop the units so that
//they can search for another building that is not at max harvesters (scv on idle)
bool BasicSc2Bot::ManageSCV() {

	
	const ObservationInterface* observation = Observation();
	Units buildings = observation->GetUnits(Unit::Alliance::Self);
	Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
		
	int numberSCVatCommandCenter = 0;
	int numberIdealSCVatCommandCenter = 0;
	int totalExtraAssigned = 0;
	int totalIdeal = 0;
	int totalAssigned = 0;
	bool atLeastOneFullBuilding = false;
	for (auto b : buildings) {
		int assigned = b->assigned_harvesters;
		int ideal = b->ideal_harvesters;
		totalAssigned += assigned;
		totalIdeal += ideal;
		if (b->unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
			//Count total scv's at command center and total ideal at command center
			numberSCVatCommandCenter += assigned;
			numberIdealSCVatCommandCenter += ideal;
		}
		if (assigned > ideal) {
			totalExtraAssigned += (assigned - ideal);
			if (b->unit_type != UNIT_TYPEID::TERRAN_COMMANDCENTER) {
				atLeastOneFullBuilding = true;
			}
		}
	}
	int scvsMoved = 0;
	//If total ideal - ideal number at command center > total assigned - actual assigned at command center && at least one command center is over capacity
	//Basically try to assign units to refineries until they are all full, then command center takes the extra guys.
	if ((totalIdeal-numberIdealSCVatCommandCenter > totalAssigned-numberSCVatCommandCenter) || atLeastOneFullBuilding) {
		//Give commands to extra scv's that are currently overflowing somewhere.
		//Assign extra number of units to another location
		for (auto unit : units) {
			//Only move extra units //Don't move too many at a time
			if ((scvsMoved < totalExtraAssigned)|| scvsMoved == 5) {
				break;
			}
			else {
				GiveSCVACommand(unit);
				++scvsMoved;
			}
		}
	}

	return true;
	
}

bool BasicSc2Bot::GiveSCVACommand(const Unit* unit) {
	const ObservationInterface* observation = Observation();
	const GameInfo& game_info = observation->GetGameInfo();
	const Unit* bestCommandCenter = botFunctions->FindBestNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER, Unit::Alliance::Self);
	const Unit* mineral_target;
	bool foundSomething = true;
	//Search for a mineral field or refinery that isn't full
	if (!bestCommandCenter) {
		mineral_target = botFunctions->FindBestNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	else {
		mineral_target = botFunctions->FindBestNearestUnit(bestCommandCenter->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	if (!mineral_target) {
		if (!bestCommandCenter) {
			foundSomething = false;
		}
		else {
			mineral_target = botFunctions->FindBestNearestUnit(bestCommandCenter->pos, UNIT_TYPEID::NEUTRAL_MINERALFIELD);
			if (!mineral_target) {
				foundSomething = false;
			}
		}
	}
	//If found an empty mineral field send it there
	if (foundSomething) {
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
	}
	//Else give it another command
	else {
		//Give a dummy command and send somewhere else. Possibly build a command center somewhere
		//We have a small army and no more mineral fields can be found so attempt to start a new base
		if ( bases.size() < 4) {
			//Find base location where a base doesn't already exist
			std::vector<Point2D> startLocations = game_info.start_locations;
			bool suitable = false;
			for (int i = 0; i < 10; i++) {
				int randomNum = rand() % startLocations.size();
				Point2D potentialLocation = startLocations[randomNum];
				//For each potential base location, if there is already a base there we keep looing.
				suitable = true;
				for (auto b : bases) {
					if (DistanceSquared2D(potentialLocation, b) < 10) {
						//there a base at that start location already
						suitable = false;
						break;
					}
				}
				if (suitable) {
					Actions()->UnitCommand(unit, ABILITY_ID::BUILD_COMMANDCENTER, potentialLocation);
					bases.push_back(potentialLocation); //Add the new base
					std::cout << "Bases size: " << bases.size() << std::endl;
					if (resetBasesNumberInNumSteps > 30) {
						resetBasesNumberInNumSteps = 0;
					}
					return true;
				}
			}
		}
		//No extra bases to create so just send it to the closest command center for managing later.
		mineral_target = botFunctions->FindNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER);
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
		
	}
	return true;

	
}	

//Check if there is still a command center at the base
//If it got destroyed then remove it from the bases
void BasicSc2Bot::resetBasesNumber() {
	
	if (resetBasesNumberInNumSteps == 20) {
		Units commandCenters = Observation()->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_COMMANDCENTER));
		for (int i = 0; i < bases.size(); i++) {
			bool stillABase = false;
			for (auto c : commandCenters) {
				//There is still a command center near that base location
				if (DistanceSquared2D(c->pos, bases[i]) < 10) {
					stillABase = true;
					break;
				}
			}
			//There is no command center near this base so remove it from our base list
			if (!stillABase) {
				std::cout << "base erased" << std::endl;
				bases.erase(bases.begin() + i);
			}
		}
	}
	else if (resetBasesNumberInNumSteps == 50) {
		resetBasesNumberInNumSteps = -1;
	}
	++resetBasesNumberInNumSteps;
}