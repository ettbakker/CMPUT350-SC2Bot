#include "ProductionManager.h"

ProductionManager::ProductionManager()
{
	
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

	Actions()->UnitCommand(builder_unit, build_ability, near_point);
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

	Actions()->UnitCommand(builder_unit, build_ability, target_unit);
	return true;
}

// Methods for verifying whether a certain structure can be built
bool ProductionManager::CanBuildSupplyDepot() {
	const ObservationInterface* observation = Observation();
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
	const ObservationInterface* observation = Observation();
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
bool ProductionManager::TryBuildRefinery(const Unit* target_geyser) {
	if (target_geyser == nullptr) {
		Point2D startPoint = Observation()->GetStartLocation();
		if (CountUnitType(UNIT_TYPEID::TERRAN_REFINERY) > 3) {
			return false;
		}

		//Need to find position of vespene gas
		const Unit* target_geyser = GetNearestUnit(startPoint, UNIT_TYPEID::NEUTRAL_VESPENEGEYSER);
		if (!target_geyser) {
			return false;
		}
	}
	return TryBuildStructureAtUnit(ABILITY_ID::BUILD_REFINERY, target_geyser);
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

// Build utility methods
Point2D ProductionManager::GetNearbyPoint(const Point2D& start_point, float build_radius)
{
	return Point2D(start_point.x + (GetRandomScalar() * build_radius),
		start_point.y + (GetRandomScalar() * build_radius));
}

const Unit* ProductionManager::GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type) {
	const ObservationInterface* observation = Observation();
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