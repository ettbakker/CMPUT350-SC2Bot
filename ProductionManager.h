#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "EconomyManager.h"

#define DEFAULT_BUILD_RADIUS 15.0

using namespace sc2;


class ProductionManager : public Manager {
public:
	ProductionManager();
	~ProductionManager();

	void BuildStructures();

	// Generic Build Structure Methods
	bool TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius = DEFAULT_BUILD_RADIUS, const Unit* builder_unit = nullptr);
	bool TryBuildStructureInBox(ABILITY_ID build_ability, const BoundingBox& box, const Unit* builder_unit = nullptr);
	bool TryBuildStructureAtUnit(ABILITY_ID build_ability, const Unit* target_unit, const Unit* builder_unit = nullptr);

	// Build Refinery Methods
	bool CanBuildRefinery();
	bool TryBuildRefinery(const Unit* target_geyser = nullptr);

	// Build Supply Depot Methods
	bool CanBuildSupplyDepot();
	bool TryBuildSupplyDepot(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildSupplyDepot(const BoundingBox& box);

	// Build Command Center Methods
	bool CanBuildCommandCenter();
	bool TryBuildCommandCenter(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildCommandCenter(const BoundingBox& box);

	// Build Barracks Methods
	bool CanBuildBarracks();
	bool TryBuildBarracks(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBarracks(const BoundingBox& box);

	// Build Engineering Bay Methods
	bool CanBuildEngineeringBay();
	bool TryBuildEngineeringBay(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildEngineeringBay(const BoundingBox& box);

	// Build Armory Methods
	bool CanBuildArmory();
	bool TryBuildArmory(float build_radius = DEFAULT_BUILD_RADIUS);
	bool TryBuildArmory(const BoundingBox& box);

	// Build Turrets Methods
	bool CanBuildTurret();
	bool TryBuildTurrets(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildTurrets(const BoundingBox& box);

	// Build Factory Methods
	bool CanBuildFactory();
	bool TryBuildFactory(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFactory(const BoundingBox& box);

	// Build Bunker Methods
	bool CanBuildBunker();
	bool TryBuildBunker(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBunker(const BoundingBox& box);

	// Build Star Port Methods
	bool CanBuildStarPort();
	bool TryBuildStarPort(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildStarPort(const BoundingBox& box);

	// Build Fusion Core Methods
	bool CanBuildFusionCore();
	bool TryBuildFusionCore(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFusionCore(const BoundingBox& box);

	// On-idle methods for buildings and SCV
	void OnIdleSCV(const Unit* unit);
	void OnIdleCommandCenter(const Unit* unit);
	void OnIdleBarracks(const Unit* unit);
	void OnIdleFactory(const Unit* unit);
	void OnIdleEngineeringBay(const Unit* unit);
	void OnIdleArmory(const Unit* unit);
	void OnIdleOrbitalCommand(const Unit* unit);
  
	void TryBuildAddOn(const Unit* unit, ABILITY_ID add_on_ability);
	bool FixBuildings();
	
	Point2D GetNearbyPoint(const Point2D& start_point, float build_radius = DEFAULT_BUILD_RADIUS);
	const Unit* GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type = UNIT_TYPEID::TERRAN_SCV);
	const Unit* FindNearestBuildableGeyser(Point2D start);
	const Unit* FindNearestMineralPatch(Point2D start);

private:
	EconomyManager* econMngr;
};