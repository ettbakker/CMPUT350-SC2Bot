#include "Command.h"

// Command class methods

Command::Command() : has_target(false), has_point(false), target(nullptr) { }

Command::Command(const Unit* unit, AbilityID ability) 
	: has_target(false), has_point(false), has_one_unit(true) {
	this->unit = unit;
	this->ability = ability;
}

Command::Command(const Unit* unit, AbilityID ability, Point2D point) 
	: has_target(false), has_point(true), has_one_unit(true) {
	this->unit = unit;
	this->ability = ability;
	this->point = point;
}

Command::Command(const Unit* unit, AbilityID ability, const Unit* target) 
	: has_target(true), has_point(false), has_one_unit(true) {
	this->unit = unit;
	this->ability = ability;
	this->target = target;
}

const Unit* Command::GetUnit() const
{
	return unit;
}

/**
Command::Command(std::vector<const Unit*> units, AbilityID ability)
	: has_target(false), has_point(false), has_one_unit(false)
{
	assert(units.size() > 0);
	this->units = units;
	this->ability = ability;

}

Command::Command(const Units& units, AbilityID ability, Point2D point)
	: Command(units, ability)
{
	this->point = point;
	has_target = false;
}

Command::Command(const Units& units, AbilityID ability, const Unit* target)
	: Command(units, ability)
{
	this->target = target;
	has_target = true;
}
*/
//Units Command::GetUnits() const { return units; }

AbilityID Command::GetAbility() const { return ability; }

bool Command::GetPoint(Point2D& point) const {
	if (has_point) { point = this->point; }
	return has_point;
}
bool Command::GetTarget(const Unit** target) const {
	if (has_target) { *target = this->target; }
	return has_target;
}

// CommandSequence class methods

void CommandSequence::PushBack(const Command& cmd) {
	std::cout << "Added cmd\n";
	commands.push_back(cmd);
}

bool CommandSequence::IsEmpty() const {
	return commands.empty();
}

Command CommandSequence::Front() const {
	return commands.front();
}
void CommandSequence::PopFront() {
	commands.pop_front();
}

void CommandSequence::Clear() {
	commands.clear();
}