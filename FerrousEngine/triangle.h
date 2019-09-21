#pragma once
#include "shapes_setup.h"
using namespace glm;

namespace fe {
	namespace shapes {
		template<typename T, qualifier Q>
		struct Triangle {
			vec<2, T, Q> a;
			vec<2, T, Q> b;
			vec<2, T, Q> c;

			Triangle() {
				a = vec<2, T, Q>((T)0, (T)0);;
				b = vec<2, T, Q>((T)0, (T)0);;
				c = vec<2, T, Q>((T)0, (T)0);;
			}

			Triangle(vec<2, T, Q> a, vec<2, T, Q> b, vec<2, T, Q> c) {
				this->a = a;
				this->b = b;
				this->c = c;
			}

			T perimeter() {
				return distance(a, b) + distance(b, c) + distance(c, a);
			}

			T area() {
				return abs((a.x * (b.y - c.y) +
					b.x * (c.y - a.y) +
					c.x * (a.y - b.y)) / 2);
			}

			vec<2, T, Q> center() {
				return vec<2, T, Q>(
					(a.x + b.x + c.x) / (T)3,
					(a.y + b.y + c.y) / (T)3
					);
			}

			// Returns the first side of the triangle, as a line from points a to b.
			Line<T, Q> sideA() {
				return Line(a, b);
			}


			// Returns the second side of the triangle, as a line from points b to c.
			Line<T, Q> sideB() {
				return Line(b, c);
			}


			// Returns the third side of the triangle, as a line from points c to a.
			Line<T, Q> sideC() {
				return Line(c, a);
			}

			static Triangle<T, Q> empty() {
				return Triangle<T, Q>(
					vec<2, T, Q>((T)0, (T)0),
					vec<2, T, Q>((T)0, (T)0),
					vec<2, T, Q>((T)0, (T)0)
					);
			}
		};
#pragma region CONTAINMENT
		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q>& t, const vec<2, T, Q> v) {
			// Compute vectors        
			vec<2, T, Q> v0 = t.c - t.a;
			vec<2, T, Q> v1 = t.b - t.a;
			vec<2, T, Q> v2 = v - t.a;

			// Compute dot products
			vec<2, T, Q> dot00 = dot(v0, v0);
			vec<2, T, Q> dot01 = dot(v0, v1);
			vec<2, T, Q> dot02 = dot(v0, v2);
			vec<2, T, Q> dot11 = dot(v1, v1);
			vec<2, T, Q> dot12 = dot(v1, v2);

			// Compute barycentric coordinates
			vec<2, T, Q> invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
			vec<2, T, Q> u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			vec<2, T, Q> v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			// Check if point is in triangle
			return (u >= 0) && (v >= 0) && (u + v < 1);
		}

		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q> & t, const T x, const T y) {
			return contains(t, vec<2, T, Q>(x, y));
		}

		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q> & t, const Rect<T, Q> & r);

		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q> & t, const Line<T, Q> & l);

		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q> & t, const Circle<T, Q> & c);

		template<typename T, qualifier Q>
		inline bool contains(const Triangle<T, Q> & t1, const Triangle<T, Q> & t2);
#pragma endregion

#pragma region INTERSECTION
		template<typename T, qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Rect<T, Q>& r) {
			return intersects(r, t);
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Circle<T, Q>& r);

		template<typename T, qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t, const Line<T, Q>& l) {
			return intersects(l, t);
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Triangle<T, Q>& t1, const Triangle<T, Q>& t2);
#pragma endregion

#pragma region OPERATORS
		template<typename T, qualifier Q>
		inline bool operator ==(const Triangle<T, Q>& l, const Triangle<T, Q>& r) {
			return l.a == r.a && l.b == r.b && l.c == r.c;
		}
#pragma endregion

#pragma region LINE TYPES
		/*16-bit signed integer triangle.*/
		typedef Triangle<int16_t, highp> Triangle16;

		/*32-bit signed integer triangle.*/
		typedef Triangle<int32_t, highp> Triangle32;

		/*64-bit integer triangle.*/
		typedef Triangle<int64_t, highp> Triangle64;

		/*16-bit unsigned integer triangle.*/
		typedef Triangle<uint16_t, highp> TriangleU16;

		/*32-bit unsigned integer triangle.*/
		typedef Triangle<uint32_t, highp> UTriangle32;

		/*64-bit unsigned integer triangle.*/
		typedef Triangle<uint64_t, highp> UTriangle64;

		/*32-bit single-precision floating-point triangle.*/
		typedef Triangle<float, defaultp> TriangleF;

		/*64-bit double-precision, floating-point triangle.*/
		typedef Triangle<double, highp> TriangleD;
#pragma endregion	
	}
}