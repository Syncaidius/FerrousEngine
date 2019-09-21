#pragma once
#include "shapes_setup.h"
#include <gtc/constants.hpp>
using namespace glm;

namespace fe {
	namespace shapes {
		template<typename T, qualifier Q>
		struct Circle {
			T radius;
			vec<2, T, Q> origin;

			Circle() {
				radius = (T)0;
				origin = vec<2, T, Q>((T)0, (T)0);
			}

			Circle(const vec<2, T, Q>& origin, const T radius) {
				this->origin = origin;
				this->radius = radius;
			}

			void inflate(const T amount) {
				radius += amount;
			}

			Circle<T, Q> getInflated(const T amount) {
				return Circle(origin, radius + amount);
			}

			double circumference() {
				return 2.0* pi<T>()* radius;
			}

			double diameter() {
				return radius * 2.0;
			}

			double area() {
				return pi<T>() * (radius * radius);
			}


			static Circle<T,Q> empty() {
				return Circle(vec<2, T, Q>((T)0, (T)0, (T)0));
			}
		};

#pragma region CONTAINMENT
		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Rect<T, Q>& r);

		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Line<T, Q>& l) {
			return (distance(c.origin, l.start) < c.radius) &&
				(distance(c.origin, l.end) < c.radius);
		}

		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c1, const Circle<T, Q>& c2);

		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const T x, const T y) {
			return distance(c.origin, vec<2, T, Q>(x, y)) <= c.radius;
		}

		template<typename T, qualifier Q>
		inline bool contains(const Circle<T, Q>& c, const vec<2, T, Q> v) {
			return distance(c.origin, v) <= c.radius;
		}
#pragma endregion

#pragma region INTERSECTION
		template<typename T, qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Line<T, Q>& l);

		template<typename T, qualifier Q>
		inline bool intersects(const Circle<T, Q>& c, const Triangle<T, Q>& t);

		template<typename T, qualifier Q>
		inline bool intersects(const Circle<T, Q>& c1, const Circle<T, Q>& c2) {
			return distance(c1.origin, c2.origin) <= (c1.radius + c2.radius);
		}
#pragma endregion

#pragma region OPERATORS
		template<typename T, qualifier Q>
		inline bool operator ==(const Circle<T, Q>& l, const Circle<T, Q>& r) {
			return l.origin == r.origin && l.radius == r.radius;
		}
#pragma endregion

#pragma region CIRCLE TYPES
		/*16-bit signed integer circle.*/
		typedef Circle<int16_t, highp> Circle16;

		/*32-bit signed integer circle.*/
		typedef Circle<int32_t, highp> Circle32;

		/*64-bit integer circle.*/
		typedef Circle<int64_t, highp> Circle64;

		/*16-bit unsigned integer circle.*/
		typedef Circle<uint16_t, highp> CircleU16;

		/*32-bit unsigned integer circle.*/
		typedef Circle<uint32_t, highp> UCircle32;

		/*64-bit unsigned integer circle.*/
		typedef Circle<uint64_t, highp> UCircle64;

		/*32-bit single-precision floating-point circle.*/
		typedef Circle<float, defaultp> CircleF;

		/*64-bit double-precision, floating-point circle.*/
		typedef Circle<double, highp> CircleD;
#pragma endregion
	}
}