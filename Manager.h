#pragma once
#include "sc2api/sc2_api.h"
#include <sc2api\sc2_unit_filters.h>
#include "BoundingBox.h"
#include "Command.h"

using namespace sc2;

class Manager : public Agent {
public:
	size_t CountUnitType(UNIT_TYPEID unit_type);
	const Unit* GetNearestUnit(const Point2D& point, UNIT_TYPEID unit_type);
	Point2D GetStartPoint();
	void SendActions();
private:
	CommandSequence commands;
};