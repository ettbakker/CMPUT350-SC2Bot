#include "Manager.h"

size_t Manager::CountUnitType(UNIT_TYPEID unit_type)
{
	return Observation()->GetUnits(Unit::Alliance::Self, IsUnit(unit_type)).size();
}

const Unit* Manager::GetNearestUnit(const Point2D& point, UNIT_TYPEID unit_type)
{
	Units units = Observation()->GetUnits(Unit::Alliance::Neutral);
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
	return Observation()->GetStartLocation();
}

void Manager::SendActions()
{
	Actions()->SendActions();
}
