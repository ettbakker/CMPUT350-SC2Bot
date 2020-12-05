#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"
#include "Base.h"

using namespace sc2;

class CombatManager : public Manager {
public:
	CombatManager();

	bool AttackEnemy();
	
	// On idle methods for the combat units.
	// What to do when doing nothing.
	void OnIdleMarine(const Unit* unit);
	void OnIdleReaper(const Unit* unit);
	void OnIdleMarauder(const Unit* unit);
	void OnIdleHellion(const Unit* unit);
	void OnIdleSeigeTank(const Unit* unit);
	
	// Utility for morhping units
	void HellionMorph(const Units units, bool attacking, Point2D to);
	void SiegeTankMorph(const Units units, bool attacking, Point2D to);
	
	Point2D halfway = Point2D(0, 0);

private:
	size_t numberIdleMarines = 0;
};