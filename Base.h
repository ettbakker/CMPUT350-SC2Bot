#pragma once
#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include <sc2api\sc2_unit_filters.h>
#include <iostream>
#include <algorithm>
#include "BoundingBox.h"

using namespace sc2;

/**
* The Base class implements a basic definition for both the bot's starting base
* and the expansions it creates.
* 
* Since our bot only uses the origin point of each base in its decision-making, this
* class has essentially no functionality in this version of the bot.
*/

class Base {
public:
	Base() { }
	Base(Point2D origin);
	Base(Point2D origin, BoundingBox region);

	Point2D origin;
	BoundingBox region;
};

typedef std::vector<Base*> Bases;