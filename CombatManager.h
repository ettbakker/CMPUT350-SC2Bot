#pragma once
#include "sc2api/sc2_api.h"
#include "BoundingBox.h"
#include "Manager.h"
#include "Command.h"

using namespace sc2;

class CombatManager : public Manager {
public:
	CombatManager();

	void OnIdleMarine(const Unit* unit);
	void OnIdleReaper(const Unit* unit);
private:
};