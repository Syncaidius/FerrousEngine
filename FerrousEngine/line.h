#pragma once
#include "shapes_setup.h"
using namespace glm;

namespace fe {
	namespace shapes {
		template<typename T, qualifier Q>
		struct Line {
			vec<2, T, Q> start;
			vec<2, T, Q> end;

			inline Line() {
				start = vec<2, T, Q>((T)0, (T)0);
				end = start;
			}

			Line(vec<2, T, Q> start, vec<2, T, Q> end) {
				start = start;
				end = end;
			}

			Line(T startX, T startY, T endX, T endY) {
				start = vec<2, T, Q>(startX, startY);
				end = vec<2, T, Q>(endX, endY);
			}

			T length() {
				return distance(start, end);
			}

			vec<2, T, Q> center() {
				return mix(start, end, 0.5);
			}

			/* Gets a point along the line, based on a percentage of the line's total length from the start point.*/
			vec<2, T, Q> pointOn(double distPercent) {
				return mix(start, end, distPercent);
			}

			static Line<T, Q> empty() {
				return distance(start, end);
			}

			/* Creates a line starting from a point of origin toward  the given direction. */
			static Line<T, Q> toDirection(vec<2, T, Q> origin, vec<2, T, Q> direction, T distance) {
				return Line<T, Q>(origin, origin + (direction * distance));
			}
		};


#pragma region INTERSECTION
		template<typename T, qualifier Q>
		inline bool intersects(const Line<T, Q>& c, const Rect<T, Q>& r) {
			return intersects(r, c);
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Line<T, Q>& l, const Circle<T, Q>& c);

		template<typename T, qualifier Q>
		inline bool intersects(const Line<T, Q>& l, const Triangle<T, Q>& t) {
			return contains(t, l) || intersects(t.sideA(), l) || intersects(t.sideB(), l) || intersects(t.sideC(), l);
		}

		template<typename T, qualifier Q>
		inline bool intersects(const Line<T, Q>& l1, const Line<T, Q>& l2) {
			// Line AB represented as a1x + b1y = c1 
			double a1 = l1.end.y - l1.start.y;
			double b1 = l1.start.x - l1.end.x;
			double c1 = a1 * (l1.start.x) + b1 * (l1.start.y);

			// Line CD represented as a2x + b2y = c2 
			double a2 = l2.end.y - l2.start.y;
			double b2 = l2.start.x - l2.end.x;
			double c2 = a2 * (l2.start.x) + b2 * (l2.start.y);

			// Calc determenant and return true if not 0.
			return (a1 * b2 - a2 * b1) != 0;
		}

		template<typename T, qualifier Q>
		inline vec<2, T, Q> getIntersectPoint(const Line<T, Q> & l1, const Line<T, Q> & l2) {
			// Line l1 represented as a1x + b1y = c1 
			double a1 = l1.end.y - l1.start.y;
			double b1 = l1.start.x - l1.end.x;
			double c1 = a1 * (l1.start.x) + b1 * (l1.start.y);

			// Line l2 represented as a2x + b2y = c2 
			double a2 = l2.end.y - l2.start.y;
			double b2 = l2.start.x - l2.end.x;
			double c2 = a2 * (l2.start.x) + b2 * (l2.start.y);

			double determinant = a1 * b2 - a2 * b1;

			if (determinant == 0)
			{
				// The lines are parallel. This is simplified 
				// by returning a pair of FLT_MAX 
				return vec<2, T, Q>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
			}
			else
			{
				double x = (b2 * c1 - b1 * c2) / determinant;
				double y = (a1 * c2 - a2 * c1) / determinant;
				return vec<2, T, Q>(x, y);
			}
		}
#pragma endregion

#pragma region OPERATORS
		template<typename T, qualifier Q>
		inline bool operator ==(const Line<T, Q>& l, const Line<T, Q>& r) {
			return l.start == r.start && l.end == r.end;
		}
#pragma endregion

#pragma region LINE TYPES
		/*16-bit signed integer line.*/
		typedef Line<int16_t, highp> Line16;

		/*32-bit signed integer line.*/
		typedef Line<int32_t, highp> Line32;

		/*64-bit integer line.*/
		typedef Line<int64_t, highp> Line64;

		/*16-bit unsigned integer line.*/
		typedef Line<uint16_t, highp> LineU16;

		/*32-bit unsigned integer line.*/
		typedef Line<uint32_t, highp> ULine32;

		/*64-bit unsigned integer line.*/
		typedef Line<uint64_t, highp> ULine64;

		/*32-bit single-precision floating-point line.*/
		typedef Line<float, defaultp> LineF;

		/*64-bit double-precision, floating-point line.*/
		typedef Line<double, highp> LineD;
#pragma endregion	
	}
}