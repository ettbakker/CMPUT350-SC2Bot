#pragma once
#include <cassert>
#include "sc2api/sc2_api.h"

using namespace sc2;

/**
* This class defines a bounding box that encloses a rectangular region of the in-game map.
* The box is defined by its top-left corner (the point p) as well as its width and height.
* 
* Since our bot currently builds units in locations near points representing our bases instead
* of within a region, this class has gone unused. It was intended to represent the region defined
* by a base and its associated structures.
*/

struct BoundingBox {
	BoundingBox() { }
	BoundingBox(const Point2D& p, float w, float h) : p(p), w(w), h(h) { }
	BoundingBox(const Point2D& top_left, const Point2D& bottom_right) : p(top_left)
	{
		assert(top_left.x < bottom_right.x&& top_left.y < bottom_right.y);
		w = bottom_right.x - top_left.x;
		h = bottom_right.y - top_left.y;
	}

	// Return a random point within the bounding box.
	Point2D RandPoint() const {
		float half_w = 0.5 * w,
			half_h = 0.5 * h,
			center_x = p.x + half_w,
			center_y = p.y + half_h;

		return Point2D(center_x + (GetRandomScalar() * half_w),
			center_y + (GetRandomScalar() * half_h));
	};

	Point2D p;
	float w = 0.0, h = 0.0;
};