#pragma once
#include "ferrous_test.hpp"
#include "shapes.h"

using namespace fe::shapes;

class TestShapes : public FerrousTest {
public:

	TestShapes() : FerrousTest("Shape Test") {}

protected:
	void onRun(Logger& log) override {
		Rect32 r1 = Rect32(100, 50, 200, 150);
		auto center1 = r1.center();
		log.writeLineF("Rect32 -- Left: %d -- Top: %d -- Right: %d -- Bottom: %d -- Width: %d -- Height: %d -- Center: %d,%d",
			r1.left,
			r1.top,
			r1.right,
			r1.bottom,
			r1.width(),
			r1.height(),
			center1.x,
			center1.y);

		RectF r2 = RectF(100.5, 25, 200, 150.1);
		auto center2 = r2.center();
		log.writeLineF("RectF -- Left: %f -- Top: %f -- Right: %f -- Bottom: %f -- Width: %f -- Height: %f -- Center: %f,%f",
			r2.left,
			r2.top,
			r2.right,
			r2.bottom,
			r2.width(),
			r2.height(),
			center2.x,
			center2.y);

		TriangleF t1 = TriangleF(vec2(5.0f, 5.0f), vec2(10.0f, 10.0f), vec2(15.0f, -15.0f));
		auto area1 = t1.area();
		log.writeLineF("TriangleF -- p1: %f,%f -- b: %f,%f -- c: %f,%f -- area: %f", t1.a.x, t1.a.y, t1.b.x, t1.b.y, t1.c.x, t1.c.y, area1);

		Triangle32 t2 = Triangle32(ivec2(5, 5), ivec2(10, 10), ivec2(15, 10));
		auto area2 = t2.area();
		log.writeLineF("Triangle32 -- p1: %d,%d -- b: %d,%d -- c: %d,%d -- area: %d", t2.a.x, t2.a.y, t2.b.x, t2.b.y, t2.c.x, t2.c.y, area2);
	}
};