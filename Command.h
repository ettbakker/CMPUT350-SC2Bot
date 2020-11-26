#pragma once
#include <vector>
#include <cassert>
#include "sc2api/sc2_api.h"

using namespace sc2;

class Command {
public:
	Command();

	Command(const Unit* unit, AbilityID ability);
	Command(const Unit* unit, AbilityID ability, Point2D point);
	Command(const Unit* unit, AbilityID ability, const Unit* target);

	Command(const Units& units, AbilityID ability);
	Command(const Units& units, AbilityID ability, Point2D point);
	Command(const Units& units, AbilityID ability, const Unit* target);

	Units GetUnits() const;
	AbilityID GetAbility() const;
	bool GetPoint(Point2D& point) const;
	bool GetTarget(const Unit** target) const;

private:
	Units units;
	AbilityID ability;
	bool has_target, has_point;

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