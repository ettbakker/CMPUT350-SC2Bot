#pragma once
#include "sc2api/sc2_api.h"
#include <sc2api\sc2_unit_filters.h>
#include "BoundingBox.h"
#include "Command.h"
#include "Base.h"

using namespace sc2;

class Manager : public Agent {
public:
	size_t CountUnitType(UNIT_TYPEID unit_type);
	const Unit* GetNearestUnit(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance = Unit::Alliance::Neutral);
	const Unit* GetBestNearestUnit(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance = Unit::Alliance::Neutral);
	Point2D GetStartPoint();
	void SetObservationAndActions(const ObservationInterface* obs, ActionInterface* act, Bases& b);
	
	const ObservationInterface* observation;
	ActionInterface* actions;
	Bases bases;
};