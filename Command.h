#pragma once
#include <vector>
#include <cassert>
#include "sc2api/sc2_api.h"
#include <iostream>

using namespace sc2;

class Command {
public:
	Command();

	Command(const Unit* unit, AbilityID ability);
	Command(const Unit* unit, AbilityID ability, Point2D point);
	Command(const Unit* unit, AbilityID ability, const Unit* target);

	/**
	Command(const Units& units, AbilityID ability);
	Command(const Units& units, AbilityID ability, Point2D point);
	Command(const Units& units, AbilityID ability, const Unit* target);
	*/

	const Unit* GetUnit() const;
	//Units GetUnits() const;
	AbilityID GetAbility() const;
	bool GetPoint(Point2D& point) const;
	bool GetTarget(const Unit** target) const;

private:
	//const Units units;
	const Unit* unit;
	AbilityID ability;
	bool has_one_unit, has_target, has_point;

	const Unit* target;
	Point2D point;
};

class CommandSequence {
public:
	void PushBack(const Command& cmd);
	bool IsEmpty() const;
	Command Front() const;
	void PopFront();
	void Clear();
private:
	std::deque<Command> commands;
};