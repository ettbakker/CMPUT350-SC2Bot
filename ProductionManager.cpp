#include "ProductionManager.h"

ProductionManager::ProductionManager()
{
	
}

void ProductionManager::BuildStructures() {

	for (auto base : bases) {
		building_point = base->origin;
		//If we can find a command center at the base
		//if (CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_COMMANDCENTER, building_point, 50) >=1) {
			//There is a command center at the start location or near it then we can build buildings around that command center.
			//Allows us to have multiple bases with their own economies
		
		//Check if no command center is near by. If there is none, try to build the command center first before anything else
		if (CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_COMMANDCENTER, building_point, 25) == 0) {
			TryBuildCommandCenter(0.0);
			continue;
		}
		else {
			TryBuildSupplyDepot();
			TryBuildRefinery();
			TryBuildBarracks();
			TryBuildEngineeringBay();
			TryBuildFactory();
			TryBuildArmory();
			TryBuildTurrets(30.0);
		}
    
		/*}
		else {
			//Build a command center at the base location before we build anything else
			TryBuildCommandCenter(5.0);
		}*/
	
		
	}
	
	
}

// Generic methods for attempting to build any structure

bool ProductionManager::TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius, const Unit* builder_unit) {
	Point2D near_point;
	bool is_valid_point = false;

	for (size_t i = 0; i < 20; ++i) {
		near_point = GetRandomNearbyPoint(building_point, build_radius);
		if (observation->IsPlacable(near_point)) {
			is_valid_point = true;
			break;
		}
	}
	
	if (!is_valid_point) {
		return false;
	}

	if (builder_unit == nullptr) {
		builder_unit = GetBuilderUnit(build_ability);
		if (builder_unit == nullptr) {
			return false;
		}
	}

	actions->UnitCommand(builder_unit, build_ability, near_point);
	return true;
}

bool ProductionManager::TryBuildStructureInBox(ABILITY_ID build_ability, const BoundingBox& box, const Unit* builder_unit) {
	return ProductionManager::TryBuildStructureNearPoint(build_ability, box.RandPoint(), 0.0, builder_unit);
}

bool ProductionManager::TryBuildStructureAtUnit(ABILITY_ID build_ability, const Unit* target_unit, const Unit* builder_unit) {
	if (builder_unit == nullptr) {
		builder_unit = GetBuilderUnit(build_ability);
	}

	if (builder_unit == nullptr) {
		return false;
	}

	actions->UnitCommand(builder_unit, build_ability, target_unit);
	return true;
}

bool ProductionManager::TryBuildStructureInBase(ABILITY_ID build_ability, const Base* base, const Unit* builder_unit)
{
	// TODO: Build w.r.t. base
	return false;
}

// Methods for verifying whether a certain structure can be built

bool ProductionManager::CanBuildRefinery() {
	if (CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_REFINERY, building_point, 100) >= 1*CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildSupplyDepot() {
	// If we are not supply capped, don't build a supply depot.
	if ((observation->GetFoodUsed() <= observation->GetFoodCap() - 20) &&
		(CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 3)) {
		return false;
	}
	/*if ((CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_SUPPLYDEPOT, building_point) >= (10*CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER))) ||
		((CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 5) && (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < bases.size())) ||
		(observation->GetFoodCap() > (bases.size() * 60))) {
		return false;
	}*/
	return true;
}

bool ProductionManager::CanBuildCommandCenter() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 3) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)>0 && CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 10) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildBarracks() {
	if ((CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 1) ||
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_BARRACKS, building_point) >= 5) ||
		((CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 4) && (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < bases.size())) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > (bases.size() * 3)) ||
		((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) && (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) >1))) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildEngineeringBay() {
	if ((CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 2) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 25) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) > 1)) {
		return false;
	}

	return true;
}

bool ProductionManager::CanBuildFactory() {
	if ((CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 2)||
		(CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 5) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > (bases.size() * 2)) ||
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_FACTORY, building_point) >= 1)) {
		return false;
	}
	
	return true;
}

bool ProductionManager::CanBuildArmory()
{
	if ((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) >= 2) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 2)) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildBunker() {
	return true;
}

bool ProductionManager::CanBuildStarPort() {
	if ((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) || 
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_STARPORT, building_point) >= 1)) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildFusionCore() {
	if ((CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) >= 1)) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildTurret() {
	//Need an engineering bay to build turrets
	if ((CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) * 2 < bases.size()) ||
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_MISSILETURRET, building_point) > 15) ||
		(building_point == bases[0]->origin)) {
		return false;
	}
	return true;
}



bool ProductionManager::TryBuildRefinery(const Unit* target_geyser) {
	if (!CanBuildRefinery()) {
		return false;
	}

	//Need to find position of vespene gas
	if (target_geyser == nullptr) {
		target_geyser = GetBestNearestUnit(building_point, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
		//std::cout << "x= " << target_geyser->pos.x << " ,y= " << target_geyser->pos.y << std::endl;
		//float distance = sqrtf(pow(building_point.x - target_geyser->pos.x, 2) + pow(building_point.y - target_geyser->pos.y, 2));
		//std::cout << distance << std::endl;
	}
	if (!target_geyser) {
		return false;
	}

	return TryBuildStructureAtUnit(ABILITY_ID::BUILD_REFINERY, target_geyser);
}



// Build Supply Depot Methods
bool ProductionManager::TryBuildSupplyDepot(float build_radius) {
	if (!CanBuildSupplyDepot()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_SUPPLYDEPOT, building_point, build_radius);
}


bool ProductionManager::TryBuildSupplyDepot(const BoundingBox& box) {
	if (!CanBuildSupplyDepot()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_SUPPLYDEPOT, box);
}

// Build Command Center Methods
bool ProductionManager::TryBuildCommandCenter(float build_radius) {
	if (!CanBuildCommandCenter()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_COMMANDCENTER, building_point, build_radius);
}

bool ProductionManager::TryBuildCommandCenter(const BoundingBox& box) {
	if (!CanBuildCommandCenter()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_COMMANDCENTER, box);
}

// Build Barracks Methods
bool ProductionManager::TryBuildBarracks(float build_radius) {
	if (!CanBuildBarracks()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BARRACKS, building_point, build_radius);
}
bool ProductionManager::TryBuildBarracks(const BoundingBox& box) {
	if (!CanBuildBarracks()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_BARRACKS, box);
}

// Build Engineering Bay Methods
bool ProductionManager::TryBuildEngineeringBay(float build_radius) {
	if (!CanBuildEngineeringBay()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ENGINEERINGBAY, bases[0]->origin, build_radius);
}

bool ProductionManager::TryBuildEngineeringBay(const BoundingBox& box) {
	if (!CanBuildEngineeringBay()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_ENGINEERINGBAY, box);
}

bool ProductionManager::TryBuildArmory(float build_radius)
{
	if (!CanBuildArmory()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ARMORY, bases[0]->origin, build_radius);
}


bool ProductionManager::TryBuildArmory(const BoundingBox& box)
{
	if (!CanBuildArmory()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_ARMORY, box);
}

// Build Turrets Methods
bool ProductionManager::TryBuildTurrets(float build_radius) {
	if (!CanBuildTurret()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_MISSILETURRET, building_point, build_radius);
}
bool ProductionManager::TryBuildTurrets(const BoundingBox& box) {
	if (!CanBuildTurret()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_MISSILETURRET, box);
}

// Build Factory Methods
bool ProductionManager::TryBuildFactory(float build_radius) {
	if (!CanBuildFactory()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FACTORY, bases[0]->origin, build_radius);
}
bool ProductionManager::TryBuildFactory(const BoundingBox& box) {
	if (!CanBuildFactory()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_FACTORY, box);
}

// Build Bunker Methods
bool ProductionManager::TryBuildBunker(float build_radius) {
	if (!CanBuildBunker()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BUNKER, bases[0]->origin, build_radius);
}
bool ProductionManager::TryBuildBunker(const BoundingBox& box) {
	if (!CanBuildBunker()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_BUNKER, box);
}

// Build Star Port Methods
bool ProductionManager::TryBuildStarPort(float build_radius) {
	if (!CanBuildStarPort()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_STARPORT, bases[0]->origin, build_radius);
}
bool ProductionManager::TryBuildStarPort(const BoundingBox& box) {
	if (!CanBuildStarPort()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_STARPORT, box);
}

// Build Fusion Core Methods
bool ProductionManager::TryBuildFusionCore(float build_radius) {
	if (!CanBuildFusionCore()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FUSIONCORE, bases[0]->origin, build_radius);
}
bool ProductionManager::TryBuildFusionCore(const BoundingBox& box) {
	if (!CanBuildFusionCore()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_FUSIONCORE, box);
}

// On-idle methods

void ProductionManager::OnIdleSCV(const Unit* unit) {

	const GameInfo& game_info = observation->GetGameInfo();
	const Unit* bestCommandCenter = GetBestNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER, Unit::Alliance::Self);
	const Unit* mineral_target;
	bool foundSomething = true;
	//Search for a mineral field or refinery that isn't full
	if (!bestCommandCenter) {
		mineral_target = GetBestNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	else {
		mineral_target = GetBestNearestUnit(bestCommandCenter->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	if (!mineral_target) {
		if (!bestCommandCenter) {
			foundSomething = false;
		}
		else {
			mineral_target = GetBestNearestUnit(bestCommandCenter->pos, UNIT_TYPEID::NEUTRAL_MINERALFIELD);
			if (!mineral_target) {
				foundSomething = false;
			}
		}
	}
	//If found an empty mineral field send it there
	if (foundSomething) {
		actions->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
	}
	//Else give it another command
	else {
		//No extra bases to create so just send it to the closest command center for managing later.
		mineral_target = GetNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER);
		actions->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);

	}
}

void ProductionManager::OnIdleCommandCenter(const Unit* unit) {

	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 2 
		&& CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 2) {
		//actions->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
	}
	//actions->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);

	//if (unit->assigned_harvesters <= unit->ideal_harvesters) {
		if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) <= (16 * CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER))) {
			actions->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
			return;
		}
	//}
}

void ProductionManager::OnIdleBarracks(const Unit* unit) {
	//Add on tags are for different states of the barracks
	//Known tags are 4389076993, 4383309827, 4369678348

	if (unit->add_on_tag == 0)
	{
		if (observation->GetArmyCount() > 30) {
			TryBuildAddOn(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
		}
	}
	/*if (observation->GetArmyCount() > (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) * 20)) {
		return;
	}*/
	
	//There's a tech lab so try and have one marauder for every 7 marines
	if ((unit->add_on_tag != 0) && (CountUnitType(UNIT_TYPEID::TERRAN_MARAUDER)*7 < CountUnitType(UNIT_TYPEID::TERRAN_MARINE))) {
		//std::cout << "Tag: " << unit->add_on_tag << std::endl;
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_MARAUDER, true);
		return;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > CountUnitType(UNIT_TYPEID::TERRAN_REAPER)) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER, true);
	}
	//std::cout << "Tag: " << unit->add_on_tag << std::endl;
	actions->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE, true);
	
}


void ProductionManager::OnIdleFactory(const Unit* unit) {
	if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > CountUnitType(UNIT_TYPEID::TERRAN_HELLION)*3 ) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_HELLION);
	}
}

void ProductionManager::OnIdleEngineeringBay(const Unit* unit) {

	auto upgrades = observation->GetUpgrades();
	if (!upgrades.empty()) {
		for (const auto& upgrade : upgrades)
		{
			// Upgrade weapons then level
			// Level 2
			if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1) {
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2);
			}
			else if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2) {
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2);
			}
			// Level 3
			else if (upgrade == UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2) {
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2);
			}
			else if (upgrade == UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3) {
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2);
			}
		}
	}
	// Level 1
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONS);
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANINFANTRYARMOR);
}

void ProductionManager::OnIdleOrbitalCommand(const Unit* unit) {
	
	actions->UnitCommand(unit, ABILITY_ID::EFFECT_CALLDOWNMULE, 
		Point2D(unit->pos.x + GetRandomScalar() * 5.0f, unit->pos.y + GetRandomScalar() * 5.0f));

	if (unit->assigned_harvesters < unit->ideal_harvesters) {
		if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < 30 * bases.size()) {
			actions->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		}
	}
}

void ProductionManager::TryBuildAddOn(const Unit* unit, ABILITY_ID add_on_ability) {

	if (CountUnitType(unit->unit_type) < 2) {
		return;
	}
	Filter unit_t = IsUnit(unit->unit_type);
	Units units = observation->GetUnits(unit_t);

	for (const auto& u : units) {
		if (u->build_progress != 1) {
			return;
		}
		actions->UnitCommand(u, add_on_ability);
	}
}

void ProductionManager::OnIdleArmory(const Unit* unit) {
	auto upgrades = observation->GetUpgrades();
	if (!upgrades.empty()) {
		for (const auto& u : upgrades)
			if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL3) {
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3,true);
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3,true);
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3,true);
				break;
			}
			else if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL2) {
				std::cout << "Reasearching" << std::endl;
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2,true);
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2,true);
				actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2,true);
				break;
			}
	}

	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1,true);
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1,true);
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1,true);
}


// Build utility methods
Point2D ProductionManager::GetNearbyPoint(const Point2D& start_point, float build_radius)
{
	return Point2D(start_point.x + (GetRandomScalar() * build_radius),
		start_point.y + (GetRandomScalar() * build_radius));
}

const Unit* ProductionManager::GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type) {
	const Unit* unit_to_build = nullptr;
	Units units = observation->GetUnits(Unit::Alliance::Self);
	Point2D startPoint = observation->GetStartLocation();
	for (const auto& unit : units)
	{
		for (const auto& order : unit->orders)
		{
			if (order.ability_id == build_ability)
			{
				return nullptr;
			}
		}
		if (unit->unit_type == builder_type)
		{
			unit_to_build = unit;
		}
	}

	return unit_to_build;
}

bool ProductionManager::fixBuildings() {
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

			actions->UnitCommand(unit_to_build, ABILITY_ID::SMART, u);

		}
	}

	return false;
}
