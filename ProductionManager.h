#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "EconomyManager.h"

#define DEFAULT_BUILD_RADIUS 15.0

using namespace sc2;

/**
* The Production Manager is responsible for all operations concerning the construction/maintenance/augmentation of
* buildings, the training of new units, and the behavior of non-army units like Barracks and SCVs.
*/

class ProductionManager : public Manager {
public:
	ProductionManager();
	~ProductionManager();

	// Intended to be called during each OnStep() call in the main bot class.
	// For each base, try to build a small subset of Terran structures.
	// Engineering bays and turrets are currently disabled to focus resources elsewhere.
	void BuildStructures();

	// Generic Build Structure Methods

	// Build a given structure within a radius of a point. Returns true if build command is successful.
	bool TryBuildStructureNearPoint(ABILITY_ID build_ability, Point2D point, float build_radius = DEFAULT_BUILD_RADIUS, const Unit* builder_unit = nullptr);

	// Build a given structure in a random point within a bounding box. Returns true if build command is successful.
	bool TryBuildStructureInBox(ABILITY_ID build_ability, const BoundingBox& box, const Unit* builder_unit = nullptr);

	// Build a given structure on top of a unit, returning true if build command succeeds. Only used when building refineries.
	bool TryBuildStructureAtUnit(ABILITY_ID build_ability, const Unit* target_unit, const Unit* builder_unit = nullptr);

	// Methods for verifying whether a certain structure can be built.
	// Each includes a resource check to see if enough minerals/gas are available at the moment.

	// Returns true if we don't already have 2 refineries per command center.
	bool CanBuildRefinery();

	// Returns true if we are within 8 units of the supply cap.
	bool CanBuildSupplyDepot();

	// Returns true if we have less than 3 command centers and we have at least 10 marines.
	bool CanBuildCommandCenter();

	// Returns true if:
	//	1. There are no more than 5 barracks near a base to force building at other bases.
	//	2. If more than 4 barracks exist, only build a new one if we have 1 command center per base.
	//	3. There are no more than 3 barracks per base.
	//	4. If we have 2 barracks, postpone any more until a factory is built.
	bool CanBuildBarracks();

	// Returns true if:
	//	1. We have at least 2 command centers, meaning 1 complete expansion from the home base.
	//	2. We have at least 25 army units.
	//	3. We don't already have an engineering bay.
	bool CanBuildEngineeringBay();

	// Returns true if:
	//	1. We have at least 1 factory to satisfy the tech tree.
	//	2. We have no more than 1 armory.
	//	3. We have at least 2 command centers (i.e. 1 successful expansion)
	bool CanBuildArmory();

	//Returns true if:
	//	1. We have at least 1 engineering bay to satisfy the tech tree.
	//	2. We have no more than 5 turrets near the building point (i.e. base).
	//	3. We are attempting to build the turret near an expansion, not the home base.
	bool CanBuildTurret();

	// Returns true if:
	//	1. We have at least 1 barracks to satisfy the tech tree
	//	2. We have at least 5 marines already.
	//	3. We have no more than 2 factories per base.
	bool CanBuildFactory();

	// Returns true if we can afford to build a bunker.
	bool CanBuildBunker();

	// Returns true if:
	//	1. We have at least 1 factory to satisfy the tech tree.
	//	2. We have no more than 1 starport within range of the base.
	bool CanBuildStarPort();

	// Returns true if we have a starport to satisfy the tech tree.
	bool CanBuildFusionCore();

	// Build methods for most Terran structures.

	// If a refinery can be built, find a nearby free geyser and build on it.
	bool TryBuildRefinery(const Unit* target_geyser = nullptr);

	bool TryBuildSupplyDepot(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildSupplyDepot(const BoundingBox& box);

	bool TryBuildCommandCenter(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildCommandCenter(const BoundingBox& box);

	bool TryBuildBarracks(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBarracks(const BoundingBox& box);

	bool TryBuildEngineeringBay(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildEngineeringBay(const BoundingBox& box);

	bool TryBuildArmory(float build_radius = DEFAULT_BUILD_RADIUS);
	bool TryBuildArmory(const BoundingBox& box);

	bool TryBuildTurrets(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildTurrets(const BoundingBox& box);

	bool TryBuildFactory(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFactory(const BoundingBox& box);

	bool TryBuildBunker(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildBunker(const BoundingBox& box);

	bool TryBuildStarPort(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildStarPort(const BoundingBox& box);

	bool TryBuildFusionCore(float build_radius=DEFAULT_BUILD_RADIUS);
	bool TryBuildFusionCore(const BoundingBox& box);

	// On-idle methods for buildings and SCV

	// Unimplemented as we currently focus on attacking the enemy before repairs are relevant.
	void OnIdleSCV(const Unit* unit);

	// Command centers will try to train 22 SCVs per center (16 for minerals, 3 for each of the 2 refineries).
	void OnIdleCommandCenter(const Unit* unit);

	// When idle, barracks will train marines, reapers, and marauders (if a tech lab has been attached).
	void OnIdleBarracks(const Unit* unit);

	// Factories will train 1 hellion per 3 marines we currently have.
	void OnIdleFactory(const Unit* unit);

	// Engineering bays will attempt to upgrade infantry abilities up to their max level.
	void OnIdleEngineeringBay(const Unit* unit);

	// Armories will attempt to improve vehicles with each of their 3 upgrades upto level 3.
	// Currently disabled to save resources for other tasks.
	void OnIdleArmory(const Unit* unit);

	// In addition to training SCVs, MULEs will be constantly called down.
	void OnIdleOrbitalCommand(const Unit* unit);
  
	// Methods for augmenting/fixing structures

	// Try to build an add-on for a given completed structure.
	void TryBuildAddOn(const Unit* unit, ABILITY_ID add_on_ability);

	// For each damaged building, dispatch 1 SCV to repair it.
	bool FixBuildings();
	
	// Utility methods

	// Return a random point within a given radius of a starting point.
	Point2D GetNearbyPoint(const Point2D& start_point, float build_radius = DEFAULT_BUILD_RADIUS);

	// Find a builder unit to execute the given build command. If another unit is already executing
	// that build command, return nullptr instead.
	const Unit* GetBuilderUnit(ABILITY_ID build_ability, UNIT_TYPEID builder_type = UNIT_TYPEID::TERRAN_SCV);

	// Return the nearest vespene geyser that we haven't built a refinery on.
	const Unit* FindNearestBuildableGeyser(Point2D start);

	// Return the closest mineral patch to a given starting point.
	const Unit* FindNearestMineralPatch(Point2D start);

	// Return the nearest refinery/command center that doesn't already have the maximum number of harvesters.
	const Unit* GetBestNearestHarvestSpot(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance);

private:
	EconomyManager* econMngr;
};