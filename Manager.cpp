#include "Manager.h"

size_t Manager::CountUnitType(UNIT_TYPEID unit_type)
{
	return observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

size_t Manager::CountUnitTypeFromPoint(UNIT_TYPEID unit_type, Point2D point, int search_radius)
{
	size_t number = 0;
	Units units = observation->GetUnits(Unit::Alliance::Self, IsUnit(unit_type));
	for (auto u : units) {
		if (Distance2D(u->pos, point) < search_radius) {
			++number;
		}
	}
	return number;
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

Point2D Manager::GetStartPoint()
{
	return observation->GetStartLocation();
}

Point2D Manager::GetRandomNearbyPoint(Point2D start_point, float distance)
{
	float rx = GetRandomScalar(), ry = GetRandomScalar();
	return Point2D(start_point.x + rx * distance, start_point.y + ry * distance);
}

void Manager::SetGameStateVars(const ObservationInterface* obs, ActionInterface* act, Bases& b, std::vector<Point3D> expLocations) {
	observation = obs;
	actions = act;
	bases = b;
	building_point = GetStartPoint();
	expansionLocations = expLocations;
}
