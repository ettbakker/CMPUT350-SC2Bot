#pragma once
#include "Manager.h"
#include "TerranUnitCosts.h"

class EconomyManager : public Manager {
public:
	EconomyManager() { };

	void UpdateResourceCounts();

	bool CanAffordBuilding(UNIT_TYPEID building_type);
	bool CanAffordUnit(UNIT_TYPEID unit_type);
	bool CanAffordUpgrade(UPGRADE_ID upgrade_type);

	int minerals=0, gas=0, food_used=0, food_cap=0;
};