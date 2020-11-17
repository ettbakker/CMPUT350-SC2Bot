#pragma once
#include <vector>
#include <cassert>
#include "sc2api/sc2_api.h"

namespace sc2 {

	class Command {
	public:
		Command(const Units& units, sc2::AbilityID ability);
		Command(const Units& units, sc2::AbilityID ability, sc2::Point2D point);
		Command(const Units& units, sc2::AbilityID ability, const sc2::Unit* target);

		Units get_units() const;
		sc2::AbilityID get_ability() const;
		bool get_point(sc2::Point2D& point) const;
		bool get_target(const sc2::Unit** target) const;

	private:
		Units units;
		sc2::AbilityID ability;
		bool has_target, has_point;

		const sc2::Unit* target;
		sc2::Point2D point;
	};

}