#include "Base.h"

Base::Base(Point2D origin) : origin(origin) {

}

Base::Base(Point2D origin, BoundingBox region) : origin(origin), region(region) {

}