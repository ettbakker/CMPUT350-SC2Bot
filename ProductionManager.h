#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"

#define DEFAULT_BUILD_RADIUS 15.0

using namespace sc2;



class ProductionManager : public Manager {
public:
	ProductionManager();

	void BuildStructures();

	bool TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius = DEFAULT_BUILD_RADIUS, const Unit* builder_unit = nullptr);
	bool TryBuildStructureInBox(ABILITY_ID build_ability, const BoundingBox& box, const Unit* builder_unit = nullptr);
	bool TryBuildStructureAtUnit(ABILITY_ID build_ability, const Unit* target_unit, const Unit* builder_unit = nullptr);
	bool TryBuildStructureInBase(ABILITY_ID build_ability, const Base* base, const Unit* builder_unit = nullptr);

	// Build Refinery Methods
	bool TryBuildRefinery(const Unit* target_geyser = nullptr);
	bool TryBuildRefinery(Point2D point, const Unit* target_geyser = nullptr);

	// Build Supply Depot Methods
	bool CanBuildSupplyDepot();
	bool TryBuildSupplyDepot(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildSupplyDepot(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildSupplyDepot(const BoundingBox& box);

	// Build Command Center Methods
	bool CanBuildCommandCenter();
	bool TryBuildCommandCenter(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildCommandCenter(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildCommandCenter(const BoundingBox& box);

	// Build Barracks Methods
	bool CanBuildBarracks();
	bool TryBuildBarracks(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBarracks(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBarracks(const BoundingBox& box);

	// Build Engineering Bay Methods
	bool CanBuildEngineeringBay();
	bool TryBuildEngineeringBay(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildEngineeringBay(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildEngineeringBay(const BoundingBox& box);

	// Build Armory Methods
	bool CanBuildArmory();
	bool TryBuildArmory(float build_radius = DEFAULT_BUILD_RADIUS);
	bool TryBuildArmory(Point2D point, float build_radius = DEFAULT_BUILD_RADIUS);
	bool TryBuildArmory(const BoundingBox& box);

	// Build Turrets Methods
	bool CanBuildTurret();
	bool TryBuildTurrets(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildTurrets(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildTurrets(const BoundingBox& box);

	// Build Factory Methods
	bool CanBuildFactory();
	bool TryBuildFactory(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFactory(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFactory(const BoundingBox& box);

	// Build Bunker Methods
	bool CanBuildBunker();
	bool TryBuildBunker(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBunker(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBunker(const BoundingBox& box);

	// Build Star Port Methods
	bool CanBuildStarPort();
	bool TryBuildStarPort(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildStarPort(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildStarPort(const BoundingBox& box);

	// Build Fusion Core Methods
	bool CanBuildFusionCore();
	bool TryBuildFusionCore(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFusionCore(Point2D point, float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFusionCore(const BoundingBox& box);

	// On-idle methods for buildings and SCV
	void OnIdleSCV(const Unit* unit);
	void OnIdleCommandCenter(const Unit* unit);
	void OnIdleBarracks(const Unit* unit);
	void OnIdleArmory(const Unit* unit);

	Point2D GetNearbyPoint(const Point2D& start_point, float build_radius = DEFAULT_BUILD_RADIUS);

	const Unit* GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type = UNIT_TYPEID::TERRAN_SCV);

	bool fixBuildings();

private:
};