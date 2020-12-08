#include "ProductionManager.h"

ProductionManager::ProductionManager()
{
	econMngr = new EconomyManager();
}

ProductionManager::~ProductionManager() {
	delete econMngr;
}

void ProductionManager::BuildStructures() {
	econMngr->SetGameStateVars(observation, actions, bases, expansionLocations);

	for (auto base : bases) {
		building_point = base->origin;
		
		// If no command center exists for this base, try build one before anything else
		if (CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_COMMANDCENTER, building_point, 25) == 0) {
			TryBuildCommandCenter(0.0);
			continue;
		}
		else {
			TryBuildSupplyDepot();
			TryBuildRefinery();
			TryBuildBarracks();
			TryBuildFactory();
			TryBuildArmory();

			// Uncomment to reenable engineering bays (upgrade vehicles) and turrets.
			//TryBuildEngineeringBay();
			//TryBuildTurrets(30.0);
		}
	}
}

// Generic Build Structure Methods

bool ProductionManager::TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius, const Unit* builder_unit) {
	Point2D near_point;
	bool is_valid_point = false;

	// Attempt to find a random placable location nearby
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

// Methods for verifying whether a certain structure can be built.

bool ProductionManager::CanBuildRefinery() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_REFINERY)) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) > 2 * CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
		return false;
	}
	if (CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_REFINERY, building_point, 20) >= 2*CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
		return false;
	}
	return true;
}


bool ProductionManager::CanBuildSupplyDepot() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_SUPPLYDEPOT)) {
		return false;
	}

	if ((observation->GetFoodUsed() <= observation->GetFoodCap() - 8)) {
		return false;
	}

	return true;
}

bool ProductionManager::CanBuildCommandCenter() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_COMMANDCENTER)) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) >= 3) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER)>0 && CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 10) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildBarracks() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_BARRACKS)) {
		return false;
	}
	if ((CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_BARRACKS, building_point) >= 5) ||
		((CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 4) && (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < bases.size())) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > (bases.size() * 3)) ||
		((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) && (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 2))) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildEngineeringBay() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_ENGINEERINGBAY)) {
		return false;
	}
	if ((CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 2) ||
		(observation->GetArmyCount() < 25) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) > 1)) {
		return false;
	}

	return true;
}

bool ProductionManager::CanBuildFactory() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_FACTORY)) {
		return false;
	}
	if ((CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_MARINE) < 5) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) > (bases.size() * 2))) {
		return false;
	}
	
	return true;
}

bool ProductionManager::CanBuildArmory()
{
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_ARMORY)) {
		return false;
	}
	if ((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) >= 2) ||
		(CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 2)) {
		return false;
	}
	return true;
}


bool ProductionManager::CanBuildBunker() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_BUNKER)) {
		return false;
	}
	return true;
}


bool ProductionManager::CanBuildStarPort() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_STARPORT)) {
		return false;
	}
	if ((CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) || 
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_STARPORT, building_point) >= 1)) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildFusionCore() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_FUSIONCORE)) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildTurret() {
	if (!econMngr->CanAffordBuilding(UNIT_TYPEID::TERRAN_MISSILETURRET)) {
		return false;
	}
	//Need an engineering bay to build turrets
	if ((CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1) ||
		(CountUnitTypeFromPoint(UNIT_TYPEID::TERRAN_MISSILETURRET, building_point) > 5) ||
		(building_point == bases[0]->origin)) {
		return false;
	}
	return true;
}

// Build Methods for each Terran structure

bool ProductionManager::TryBuildRefinery(const Unit* target_geyser) {
	if (!CanBuildRefinery()) {
		return false;
	}

	//Need to find position of vespene gas
	if (target_geyser == nullptr) {
		target_geyser = FindNearestBuildableGeyser(building_point);
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
	const Unit* bestCommandCenter = GetBestNearestHarvestSpot(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER, Unit::Alliance::Self);
	const Unit* mineral_target;
	bool foundSomething = true;
	//Search for a mineral field or refinery that isn't full
	if (!bestCommandCenter) {
		mineral_target = GetBestNearestHarvestSpot(unit->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	else {
		mineral_target = GetBestNearestHarvestSpot(bestCommandCenter->pos, UNIT_TYPEID::TERRAN_REFINERY, Unit::Alliance::Self);
	}
	if (!mineral_target) {
		if (!bestCommandCenter) {
			foundSomething = false;
		}
		else {
			mineral_target = FindNearestMineralPatch(bestCommandCenter->pos);
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

	// Orbital command morph disabled as MULEs are not used by the bot.
	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) > 2 
		&& CountUnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND) < 2) {
		//actions->UnitCommand(unit, ABILITY_ID::MORPH_ORBITALCOMMAND);
	}


	if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) <= (22 * CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER))) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
		return;
	}

}

void ProductionManager::OnIdleBarracks(const Unit* unit) {
	//Add on tags are for different states of the barracks
	//Known tags are 4389076993, 4383309827, 4369678348

	// Build techlab for creating Marauders after our army size reaches 30
	if (unit->add_on_tag == 0 && observation->GetArmyCount() > 30) {
			TryBuildAddOn(unit, ABILITY_ID::BUILD_TECHLAB_BARRACKS);
	}
	
	//There's a tech lab so try and have one marauder for every 7 marines
	if ((unit->add_on_tag != 0) && (CountUnitType(UNIT_TYPEID::TERRAN_MARAUDER)*7 < CountUnitType(UNIT_TYPEID::TERRAN_MARINE))) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_MARAUDER, true);
		return;
	}
	// Maintain a ratio of 3 marines to every 2 reapers.
	if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE)*3 > CountUnitType(UNIT_TYPEID::TERRAN_REAPER)*2) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER, true);
	}

	actions->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE, true);	
}

void ProductionManager::OnIdleFactory(const Unit* unit) {
	if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > CountUnitType(UNIT_TYPEID::TERRAN_HELLIONTANK)*3 )
	{
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
	Units units;

	// Wait until we have at least 2 of the structure before upgrading them.
	if (CountUnitType(unit->unit_type) < 2) {
		return;
	}
	units = observation->GetUnits(IsUnit(unit->unit_type));

	for (const auto& u : units) {
		if (u->build_progress != 1) {
			return;
		}
		actions->UnitCommand(u, add_on_ability);
	}
}

// Currently disabled so bot can use resources for other tasks.
void ProductionManager::OnIdleArmory(const Unit* unit) {
	return; // Comment out to turn on armory upgrades.

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

bool ProductionManager::FixBuildings() {
	Units units = observation->GetUnits();

	for (auto u : units) {
		if (u->health < u->health_max) {
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

const Unit* ProductionManager::FindNearestBuildableGeyser(Point2D start)
{
	Units geysers = observation->GetUnits(Unit::Alliance::Neutral, IsGeyser());
	Units refineries = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
	float dist, closest_dist = std::numeric_limits<float>::max();
	const Unit* target_geyser = nullptr;
	bool is_refinery = false;

	for (auto g : geysers) {
		is_refinery = false;
		for (auto r : refineries) {
			if (DistanceSquared2D(r->pos, g->pos) < 5.0) {
				is_refinery = true;
				break;
			}
		}

		if (is_refinery) {
			continue;
		}

		dist = DistanceSquared2D(g->pos, start);
		if (dist < closest_dist) {
			closest_dist = dist;
			target_geyser = g;
		}
	}

	return target_geyser;
}

const Unit* ProductionManager::FindNearestMineralPatch(Point2D start) {
	Units patches = observation->GetUnits(Unit::Alliance::Neutral, IsMineralPatch());
	const Unit* target_patch = nullptr;
	float dist, closest_dist = std::numeric_limits<float>::max();

	for (auto p : patches) {
		dist = DistanceSquared2D(p->pos, start);
		if (dist < closest_dist) {
			closest_dist = dist;
			target_patch = p;
		}
	}

	return target_patch;
}

const Unit* ProductionManager::GetBestNearestHarvestSpot(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance)
{
	Units units = observation->GetUnits(alliance);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units)
	{
		if (u->unit_type == unit_type)
		{
			//If refinery is full, find another one
			if (unit_type == UNIT_TYPEID::TERRAN_REFINERY) {
				if (u->assigned_harvesters >= 3) {
					continue;
				}
				else if (u->vespene_contents <= 0) {
					continue;
				}
			}

			if (unit_type == UNIT_TYPEID::TERRAN_COMMANDCENTER) {
				if (u->assigned_harvesters >= 16) {
					continue;
				}
			}

			float d = DistanceSquared2D(u->pos, point);
			if (d < distance)
			{
				distance = d;
				target = u;
			}
		}
	}
	return target;
}