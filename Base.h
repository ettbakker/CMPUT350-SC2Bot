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


class Base {
public:
	Base() { }
	Base(Point2D origin);
	Base(Point2D origin, BoundingBox region);

	Point2D origin;
	BoundingBox region;
};

typedef std::vector<Base*> Bases;