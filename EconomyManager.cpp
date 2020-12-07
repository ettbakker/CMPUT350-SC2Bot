#include "EconomyManager.h"

void EconomyManager::UpdateResourceCounts() {
	minerals = observation->GetMinerals();
	gas = observation->GetVespene();
	food_used = observation->GetFoodUsed();
	food_cap = observation->GetFoodCap();
}

bool EconomyManager::CanAffordBuilding(UNIT_TYPEID building_type)
{
	BuildingCostTuple cost = TerranUnitCosts::BUILDINGS().at(building_type);

	UpdateResourceCounts();
	if (std::get<MINERALS>(cost) > minerals || std::get<GAS>(cost) > gas) {
		return false;
	}

	return true;
}

bool EconomyManager::CanAffordUnit(UNIT_TYPEID unit_type)
{
	UnitCostTuple cost = TerranUnitCosts::UNITS().at(unit_type);

	UpdateResourceCounts();
	if (std::get<MINERALS>(cost) > minerals ||
		std::get<GAS>(cost) > gas ||
		std::get<SUPPLY>(cost) > (food_cap - food_used)) 
	{
		return false;
	}

	return true;
}

bool EconomyManager::CanAffordUpgrade(UPGRADE_ID upgrade_type)
{
	UpgradeCostTuple cost = TerranUnitCosts::UPGRADES().at(upgrade_type);

	UpdateResourceCounts();
	if (std::get<MINERALS>(cost) > minerals || std::get<GAS>(cost) > gas) {
		return false;
	}

	return true;
}