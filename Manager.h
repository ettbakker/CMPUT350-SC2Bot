#pragma once
#include "sc2api/sc2_api.h"
#include <sc2api\sc2_unit_filters.h>
#include "BoundingBox.h"
#include "Base.h"

using namespace sc2;

/**
* The Manager superclass defines the basic structure of each Manager class used by the bot.
* 
* Managers are each given access to game info by the Sam_Sc2Bot class including Observation,
* Actions, bases, and expansion locations. These variables are intended to be updated either on
* each in-game step or whenever a unit's OnIdle method is called to ensure information is recent.
* 
* This class defines basic methods for querying unit counts and points that each Manager may use.
*/

class Manager : public Agent {
public:
	// Return the count of a friendly unit type
	size_t CountUnitType(UNIT_TYPEID unit_type); 

	//Count the number of units within a specific radius of a point
	size_t CountUnitTypeFromPoint(UNIT_TYPEID unit_type, Point2D point, int search_radius = 30);

	// Return the unit of unit_type closest to the given point.
	const Unit* GetNearestUnit(const Point2D& point, UNIT_TYPEID unit_type, Unit::Alliance alliance = Unit::Alliance::Neutral);

	// Return the starting location of our base
	Point2D GetStartPoint();

	// Return a random point with the given distance of a starting point.
	Point2D GetRandomNearbyPoint(Point2D start_point, float distance = 5.0f);

	// Used to pass certain game info accessible only to SAM_Sc2Bot to managers.
	void SetGameStateVars(const ObservationInterface* obs, ActionInterface* act, Bases& b, std::vector<Point3D> expLocations);
	
	const ObservationInterface* observation;
	ActionInterface* actions;
	Bases bases;
	Point2D building_point;
	std::vector<Point3D> expansionLocations;
	
};