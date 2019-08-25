#pragma once
#include "shapes_setup.h"

namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Triangle {
			glm::vec<2, T, Q> a;
			glm::vec<2, T, Q> b;
			glm::vec<2, T, Q> c;

			Triangle() {
				a = glm::vec<2, T, Q>((T)0, (T)0);;
				b = glm::vec<2, T, Q>((T)0, (T)0);;
				c = glm::vec<2, T, Q>((T)0, (T)0);;
			}

			Triangle(glm::vec<2, T, Q> a, glm::vec<2, T, Q> b, glm::vec<2, T, Q> c) {
				this->a = a;
				this->b = b;
				this->c = c;
			}

			T perimeter() {
				return glm::distance(a, b) + glm::distance(b, c) + glm::distance(c, a);
			}

			T area() {
				return glm::abs((a.x * (b.y - c.y) +
					b.x * (c.y - a.y) +
					c.x * (a.y - b.y)) / 2);
			}

			glm::vec<2, T, Q> center() {
				return glm::vec<2, T, Q>(
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
					glm::vec<2, T, Q>((T)0, (T)0),
					glm::vec<2, T, Q>((T)0, (T)0),
					glm::vec<2, T, Q>((T)0, (T)0)
					);
			}
		};

#pragma region OPERATORS
		template<typename T, glm::qualifier Q>
		inline bool operator ==(const Triangle<T, Q>& l, const Triangle<T, Q>& r) {
			return l.a == r.a && l.b == r.b && l.c == r.c;
		}
#pragma endregion

#pragma region LINE TYPES
		/*16-bit signed integer triangle.*/
		typedef Triangle<int16_t, glm::highp> Triangle16;

		/*32-bit signed integer triangle.*/
		typedef Triangle<int32_t, glm::highp> Triangle32;

		/*64-bit integer triangle.*/
		typedef Triangle<int64_t, glm::highp> Triangle64;

		/*16-bit unsigned integer triangle.*/
		typedef Triangle<uint16_t, glm::highp> TriangleU16;

		/*32-bit unsigned integer triangle.*/
		typedef Triangle<uint32_t, glm::highp> UTriangle32;

		/*64-bit unsigned integer triangle.*/
		typedef Triangle<uint64_t, glm::highp> UTriangle64;

		/*32-bit single-precision floating-point triangle.*/
		typedef Triangle<float, glm::defaultp> TriangleF;

		/*64-bit double-precision, floating-point triangle.*/
		typedef Triangle<double, glm::highp> TriangleD;
#pragma endregion	
	}
}