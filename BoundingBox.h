#pragma once
#include <cassert>
#include "sc2api/sc2_api.h"

using namespace sc2;


struct BoundingBox {
	BoundingBox() { }
	BoundingBox(const Point2D& p, float w, float h) : p(p), w(w), h(h) { }
	BoundingBox(const Point2D& top_left, const Point2D& bottom_right) : p(top_left)
	{
		assert(top_left.x < bottom_right.x&& top_left.y < bottom_right.y);
		w = bottom_right.x - top_left.x;
		h = bottom_right.y - top_left.y;
	}

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