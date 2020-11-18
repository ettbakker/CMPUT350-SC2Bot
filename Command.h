#pragma once
#include <vector>
#include <cassert>
#include "sc2api/sc2_api.h"


class Command {
public:
	Command(const sc2::Units& units, sc2::AbilityID ability);
	Command(const sc2::Units& units, sc2::AbilityID ability, sc2::Point2D point);
	Command(const sc2::Units& units, sc2::AbilityID ability, const sc2::Unit* target);

	sc2::Units get_units() const;
	sc2::AbilityID get_ability() const;
	bool get_point(sc2::Point2D& point) const;
	bool get_target(const sc2::Unit** target) const;

private:
	sc2::Units units;
	sc2::AbilityID ability;
	bool has_target, has_point;

	const sc2::Unit* target;
	sc2::Point2D point;
};

class CommandSequence {
public:
	void pushBack(const Command& cmd);
	bool isEmpty() const;
	Command front() const;
	void popFront();
private:
	std::deque<Command> commands;
};