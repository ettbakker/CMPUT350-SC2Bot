#include "ProductionManager.h"

ProductionManager::ProductionManager()
{
	
}

void ProductionManager::BuildStructures() {
	Point2D building_point;

	for (auto base : bases) {
		const Unit* commandCenterAtLocation = GetNearestUnit(base->origin, UNIT_TYPEID::TERRAN_COMMANDCENTER, Unit::Alliance::Self);
		if (!commandCenterAtLocation) {
			continue;
		}
		else if (DistanceSquared2D(commandCenterAtLocation->pos, base->origin) < 10) {
			//There is a command center at the start location or near it then we can build buildings around that command center.
			//Allows us to have multiple bases with their own economies
			building_point = base->origin;
			TryBuildSupplyDepot(building_point);
			TryBuildBarracks(building_point);
			TryBuildCommandCenter(building_point);
			TryBuildEngineeringBay(building_point);
			TryBuildFactory(building_point);
			TryBuildArmory(building_point);
			TryBuildTurrets(building_point);
			TryBuildRefinery(building_point);

		}
	}
}

// Generic methods for attempting to build any structure

bool ProductionManager::TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius, const Unit* builder_unit) {
	Point2D near_point(point.x + (GetRandomScalar() * build_radius),
		point.y + (GetRandomScalar() * build_radius));
	if (builder_unit == nullptr) {
		builder_unit = GetBuilderUnit(build_ability);
	}

	if (builder_unit == nullptr) {
		return false;
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
bool ProductionManager::CanBuildSupplyDepot() {
	// If we are not supply capped, don't build a supply depot.
	if (observation->GetFoodUsed() <= observation->GetFoodCap() - 2) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildCommandCenter() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) > 2) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildBarracks() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 3)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) > 7)
	{
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildEngineeringBay() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_SUPPLYDEPOT) < 5)
	{
		return false;
	}

	if (CountUnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER) < 1) {
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildFactory() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_BARRACKS) < 1)
	{
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildBunker() {
	return true;
}

bool ProductionManager::CanBuildStarPort() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1)
	{
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildFusionCore() {
	if (CountUnitType(UNIT_TYPEID::TERRAN_STARPORT) < 1)
	{
		return false;
	}
	return true;
}

bool ProductionManager::CanBuildTurret() {
	//Need an engineering bay to build turrets
	if (CountUnitType(UNIT_TYPEID::TERRAN_ENGINEERINGBAY) < 1)
	{
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_MISSILETURRET) > 30) {
		return false;
	}
	return true;
}

// Build Refinery Methods
bool ProductionManager::TryBuildRefinery(Point2D point, const Unit* target_geyser) {
	if (target_geyser == nullptr) {
		Units units = observation->GetUnits(Unit::Alliance::Neutral);
		Units refineries = observation->GetUnits(Unit::Alliance::Self, IsVisibleGeyser());
		std::vector<Point2D> r_points;
		float distance = std::numeric_limits<float>::max();

		for (const auto& u : refineries) {
			r_points.push_back(u->pos);
		}

		for (const auto& u : units) {
			if (u->unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
				if (std::find(r_points.begin(), r_points.end(), u->pos) != r_points.end()) {
					continue;
				}
				float d = DistanceSquared2D(u->pos, point);
				if (d < distance) {
					distance = d;
					target_geyser = u;
				}
			}
		}
	}
	return TryBuildStructureAtUnit(ABILITY_ID::BUILD_REFINERY, target_geyser);
}

bool ProductionManager::TryBuildRefinery(const Unit* target_geyser)
{
	return TryBuildRefinery(GetStartPoint(), target_geyser);
}

// Build Supply Depot Methods
bool ProductionManager::TryBuildSupplyDepot(float build_radius) {
	if (!CanBuildSupplyDepot()) {
		return false;
	}
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_SUPPLYDEPOT, point, build_radius);
}

bool ProductionManager::TryBuildSupplyDepot(Point2D point, float build_radius) {
	if (!CanBuildSupplyDepot()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_SUPPLYDEPOT, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_COMMANDCENTER, point, build_radius);
}
bool ProductionManager::TryBuildCommandCenter(Point2D point, float build_radius) {
	if (!CanBuildCommandCenter()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_COMMANDCENTER, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BARRACKS, point, build_radius);
}
bool ProductionManager::TryBuildBarracks(Point2D point, float build_radius) {
	if (!CanBuildBarracks()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BARRACKS, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ENGINEERINGBAY, point, build_radius);
}
bool ProductionManager::TryBuildEngineeringBay(Point2D point, float build_radius) {
	if (!CanBuildEngineeringBay()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ENGINEERINGBAY, point, build_radius);
}
bool ProductionManager::TryBuildEngineeringBay(const BoundingBox& box) {
	if (!CanBuildEngineeringBay()) {
		return false;
	}
	return TryBuildStructureInBox(ABILITY_ID::BUILD_ENGINEERINGBAY, box);
}

bool ProductionManager::CanBuildArmory()
{
	if (CountUnitType(UNIT_TYPEID::TERRAN_FACTORY) < 1) {
		return false;
	}
	if (CountUnitType(UNIT_TYPEID::TERRAN_ARMORY) >= 2) {
		return false;
	}
	return true;
}

bool ProductionManager::TryBuildArmory(float build_radius)
{
	if (!CanBuildArmory()) {
		return false;
	}
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ARMORY, point, build_radius);
}

bool ProductionManager::TryBuildArmory(Point2D point, float build_radius)
{
	if (!CanBuildArmory()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_ARMORY, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_MISSILETURRET, point, build_radius);
}
bool ProductionManager::TryBuildTurrets(Point2D point, float build_radius) {
	if (!CanBuildTurret()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_MISSILETURRET, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FACTORY, point, build_radius);
}
bool ProductionManager::TryBuildFactory(Point2D point, float build_radius) {
	if (!CanBuildFactory()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FACTORY, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BUNKER, point, build_radius);
}
bool ProductionManager::TryBuildBunker(Point2D point, float build_radius) {
	if (!CanBuildBunker()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_BUNKER, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_STARPORT, point, build_radius);
}
bool ProductionManager::TryBuildStarPort(Point2D point, float build_radius) {
	if (!CanBuildStarPort()){
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_STARPORT, point, build_radius);
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
	Point2D point = GetStartPoint();
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FUSIONCORE, point, build_radius);
}
bool ProductionManager::TryBuildFusionCore(Point2D point, float build_radius) {
	if (!CanBuildFusionCore()) {
		return false;
	}
	return TryBuildStructureNearPoint(ABILITY_ID::BUILD_FUSIONCORE, point, build_radius);
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
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
	}
	//Else give it another command
	else {
		//Give a dummy command and send somewhere else. Possibly build a command center somewhere
		if (bases.size() < 3) {
			int j = 0;
			Point2D potentialLocation;
			while (j < 50) {
				j++;
				float rx = GetRandomScalar();
				float ry = GetRandomScalar();
				potentialLocation = Point2D(bases[0]->origin.x + rx * 60.0f, bases[0]->origin.y + ry * 60.0f);
				if ((DistanceSquared2D(potentialLocation, bases[0]->origin) > 3200) && (potentialLocation.x > 40) && (potentialLocation.y > 40) && (potentialLocation.x < 110) && (potentialLocation.y < 110)) {
					Actions()->UnitCommand(unit, ABILITY_ID::BUILD_COMMANDCENTER, potentialLocation);
					bases.push_back(new Base(potentialLocation));
					std::cout << "Trying to add new base " << "X: " << potentialLocation.x << " Y: " << potentialLocation.y << std::endl;
					break;
				}
			}
		}

		//No extra bases to create so just send it to the closest command center for managing later.
		mineral_target = GetNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_COMMANDCENTER);
		Actions()->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);

	}
	


	const Unit* mineral_target = GetNearestUnit(unit->pos, UNIT_TYPEID::TERRAN_REFINERY);
	if (!mineral_target) {
		mineral_target = GetNearestUnit(unit->pos, UNIT_TYPEID::NEUTRAL_MINERALFIELD);
		if (!mineral_target) {
			return;
		}
	}
	
	actions->UnitCommand(unit, ABILITY_ID::SMART, mineral_target);
}

void ProductionManager::OnIdleCommandCenter(const Unit* unit) {
	if (CountUnitType(UNIT_TYPEID::TERRAN_SCV) < 30 * bases.size()) {
		actions->UnitCommand(unit, ABILITY_ID::TRAIN_SCV);
	}
}

void ProductionManager::OnIdleBarracks(const Unit* unit) {
	/*if (CountUnitType(UNIT_TYPEID::TERRAN_MARINE) > CountUnitType(UNIT_TYPEID::TERRAN_REAPER)) {
	Actions()->UnitCommand(unit, ABILITY_ID::TRAIN_REAPER);
	}*/
	actions->UnitCommand(unit, ABILITY_ID::TRAIN_MARINE);
}

void ProductionManager::OnIdleArmory(const Unit* unit) {
	std::vector<UpgradeID> upgrades = observation->GetUpgrades();


	for (auto u : upgrades) {
		if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL3) {
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3);
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3);
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3);
			break;
		}
		if (u == UPGRADE_ID::TERRANVEHICLEANDSHIPWEAPONSLEVEL2) {
			std::cout << "Reasearching" << std::endl;
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2);
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2);
			actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2);
			break;
		}
	}

	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1);
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1);
	actions->UnitCommand(unit, ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1);
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
