#include "Command.h"

namespace sc2 {

	Command::Command(const Units& units, sc2::AbilityID ability)
		: has_target(false), has_point(false)
	{
		assert(units.size() > 0);
		this->units = units;
		this->ability = ability;

	}

	Command::Command(const Units& units, sc2::AbilityID ability, sc2::Point2D point)
		: Command(units, ability)
	{
		this->point = point;
		has_target = false;
	}

	Command::Command(const Units& units, sc2::AbilityID ability, const sc2::Unit* target)
		: Command(units, ability)
	{
		this->target = target;
		has_target = true;
	}

	Units Command::get_units() const { return units; }

	sc2::AbilityID Command::get_ability() const { return ability; }

	bool Command::get_point(sc2::Point2D& point) const {
		if (has_point) { point = this->point; }
		return has_point;
	}
	bool Command::get_target(const sc2::Unit** target) const {
		if (has_target) { *target = this->target; }
		return has_target;
	}

}