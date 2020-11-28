#include "Manager.h"

size_t Manager::CountUnitType(UNIT_TYPEID unit_type)
{
	return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

const Unit* Manager::GetNearestUnit(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance)
{
	Units units = observation->GetUnits(alliance);
	float distance = std::numeric_limits<float>::max();
	const Unit* target = nullptr;
	for (const auto& u : units)
	{
		if (u->unit_type == unit_type)
		{
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

const Unit* Manager::GetBestNearestUnit(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance)
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

			if (unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER) {
				//When trying to build on a geyser find out if a refinrey is already build on it by getting the distance to refineries
				Units refineries = observation->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_REFINERY));
				bool already = false;
				for (auto r : refineries) {
					if (DistanceSquared2D(u->pos, r->pos) < 5.0) {
						already = true;
						break;
					}
				}
				if (already) {
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

Point2D Manager::GetStartPoint()
{
	return observation->GetStartLocation();
}

void Manager::SetObservationAndActions(const ObservationInterface* obs, ActionInterface* act, Bases& b) {
	observation = obs;
	actions = act;
	bases = b;
}
