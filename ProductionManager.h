#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"

using namespace sc2;

class ProductionManager : public Manager {
public:
	ProductionManager();

	bool TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius = 0.0f, const Unit* builder_unit = nullptr);
	bool TryBuildStructureInBox(ABILITY_ID build_ability, const BoundingBox& box, const Unit* builder_unit = nullptr);
	bool TryBuildStructureAtUnit(ABILITY_ID build_ability, const Unit* target_unit, const Unit* builder_unit = nullptr);

	// Build Refinery Methods
	bool TryBuildRefinery(const Unit* target_geyser = nullptr);

	// Build Supply Depot Methods
	bool CanBuildSupplyDepot();
	bool TryBuildSupplyDepot(float build_radius=0.0);
	bool TryBuildSupplyDepot(Point2D point, float build_radius=0.0);
	bool TryBuildSupplyDepot(const BoundingBox& box);

	// Build Command Center Methods
	bool CanBuildCommandCenter();
	bool TryBuildCommandCenter(float build_radius=0.0);
	bool TryBuildCommandCenter(Point2D point, float build_radius=0.0);
	bool TryBuildCommandCenter(const BoundingBox& box);

	// Build Barracks Methods
	bool CanBuildBarracks();
	bool TryBuildBarracks(float build_radius=0.0);
	bool TryBuildBarracks(Point2D point, float build_radius=0.0);
	bool TryBuildBarracks(const BoundingBox& box);

	// Build Engineering Bay Methods
	bool CanBuildEngineeringBay();
	bool TryBuildEngineeringBay(float build_radius=0.0);
	bool TryBuildEngineeringBay(Point2D point, float build_radius=0.0);
	bool TryBuildEngineeringBay(const BoundingBox& box);

	// Build Turrets Methods
	bool CanBuildTurret();
	bool TryBuildTurrets(float build_radius=0.0);
	bool TryBuildTurrets(Point2D point, float build_radius=0.0);
	bool TryBuildTurrets(const BoundingBox& box);

	// Build Factory Methods
	bool CanBuildFactory();
	bool TryBuildFactory(float build_radius=0.0);
	bool TryBuildFactory(Point2D point, float build_radius=0.0);
	bool TryBuildFactory(const BoundingBox& box);

	// Build Bunker Methods
	bool CanBuildBunker();
	bool TryBuildBunker(float build_radius=0.0);
	bool TryBuildBunker(Point2D point, float build_radius=0.0);
	bool TryBuildBunker(const BoundingBox& box);

	// Build Star Port Methods
	bool CanBuildStarPort();
	bool TryBuildStarPort(float build_radius=0.0);
	bool TryBuildStarPort(Point2D point, float build_radius=0.0);
	bool TryBuildStarPort(const BoundingBox& box);

	// Build Fusion Core Methods
	bool CanBuildFusionCore();
	bool TryBuildFusionCore(float build_radius=0.0);
	bool TryBuildFusionCore(Point2D point, float build_radius=0.0);
	bool TryBuildFusionCore(const BoundingBox& box);

	Point2D GetNearbyPoint(const Point2D& start_point, float build_radius = 0.0);

	const Unit* GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type = UNIT_TYPEID::TERRAN_SCV);

private:
};