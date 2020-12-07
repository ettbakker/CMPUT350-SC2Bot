#pragma once
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <unordered_map>
#include <tuple>

using namespace sc2;

/**
* This file includes classes and types for managing the overall economy of the bot, namely tracking the costs of
* building/training/upgrading units. UNIT_TYPEIDs can be supplied to their relevant classes to obtain tuples 
* representing the build/train/upgrade operation's cost in minerals and gas (and supply where applicable).
*/

enum COST { MINERALS = 0, GAS = 1, SUPPLY = 2 };

typedef std::tuple<size_t, size_t, size_t> UnitCostTuple;
typedef std::tuple<size_t, size_t> BuildingCostTuple;
typedef std::tuple<size_t, size_t> UpgradeCostTuple;

class TerranUnitCosts {
public:
	typedef std::unordered_map<UNIT_TYPEID, UnitCostTuple> UnitCostMap;
	typedef std::unordered_map<UNIT_TYPEID, BuildingCostTuple> BuildingCostMap;
	typedef std::unordered_map<UPGRADE_ID, UpgradeCostTuple> UpgradeCostMap;

	// Returns a map of building types to their cost in minerals and gas.
	static const BuildingCostMap BUILDINGS() {
		return { { UNIT_TYPEID::TERRAN_COMMANDCENTER, {400, 0} },
			{ UNIT_TYPEID::TERRAN_SUPPLYDEPOT, {100, 0} },
			{ UNIT_TYPEID::TERRAN_BARRACKS, {150, 0} },
			{ UNIT_TYPEID::TERRAN_FACTORY, {150, 100} },
			{ UNIT_TYPEID::TERRAN_ARMORY, {150, 100} },
			{ UNIT_TYPEID::TERRAN_STARPORT, {150, 100} },
			{ UNIT_TYPEID::TERRAN_GHOSTACADEMY, {150, 50} },
			{ UNIT_TYPEID::TERRAN_BUNKER, {100, 0} },
			{ UNIT_TYPEID::TERRAN_ENGINEERINGBAY, {125, 0} },
			{ UNIT_TYPEID::TERRAN_MISSILETURRET, {100, 0} },
			{ UNIT_TYPEID::TERRAN_SENSORTOWER, {125, 100} },
			{ UNIT_TYPEID::TERRAN_ORBITALCOMMAND, {550, 0} },
			{ UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, {550, 150} },
			{ UNIT_TYPEID::TERRAN_FUSIONCORE, {150, 150} },
			{ UNIT_TYPEID::TERRAN_TECHLAB, {50, 25} },
			{ UNIT_TYPEID::TERRAN_REACTOR, {50, 50} },
			{ UNIT_TYPEID::TERRAN_REFINERY, {75,0} }
		};
	}

	// Returns a map of upgrade types to their cost in minerals and gas.
	// Currently unused.
	static const UpgradeCostMap UPGRADES() {
		return {

		};
	}

	// Returns a map to the cost of units in minerals, gas, and supply.
	static const UnitCostMap UNITS() {
		return { { UNIT_TYPEID::TERRAN_SCV, {50, 0, 1} },
			{ UNIT_TYPEID::TERRAN_MARINE, {50, 0, 1} },
			{ UNIT_TYPEID::TERRAN_MARAUDER, {100, 25, 2} },
			{ UNIT_TYPEID::TERRAN_REAPER, {50, 50, 1} },
			{ UNIT_TYPEID::TERRAN_GHOST, {150, 125, 2} },
			{ UNIT_TYPEID::TERRAN_HELLION, {100, 0, 0} },
			{ UNIT_TYPEID::TERRAN_HELLIONTANK, {100, 0, 0} },
			{ UNIT_TYPEID::TERRAN_SIEGETANK, {150, 125, 3} },
			{ UNIT_TYPEID::TERRAN_CYCLONE, {150, 100, 3} },
			{ UNIT_TYPEID::TERRAN_WIDOWMINE, {75, 25, 2} },
			{ UNIT_TYPEID::TERRAN_THOR, {300, 200, 6} },
			{ UNIT_TYPEID::TERRAN_VIKINGFIGHTER, {150, 75, 2} },
			{ UNIT_TYPEID::TERRAN_MEDIVAC, {100, 100, 2} },
			{ UNIT_TYPEID::TERRAN_LIBERATOR, {150, 150, 3} },
			{ UNIT_TYPEID::TERRAN_RAVEN, {100, 200, 2} },
			{ UNIT_TYPEID::TERRAN_BANSHEE, {150, 100, 3} },
			{ UNIT_TYPEID::TERRAN_BATTLECRUISER, {400, 300, 6} }
		};
	}
};