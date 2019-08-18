#pragma once
#include <glm.hpp>

namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Triangle {
			glm::vec<2, T, Q> p1;
			glm::vec<2, T, Q> p2;
			glm::vec<2, T, Q> p3;

			Triangle();
			Triangle(glm::vec<2, T, Q> p1, glm::vec<2, T, Q> p2, glm::vec<2, T, Q> p3);

			T perimeter();
			T area();

			glm::vec<2, T, Q> center();

			static Triangle<T, Q> empty();
		};

#pragma region IMPLEMENTATION
		template<typename T, glm::qualifier Q>
		inline Triangle<T, Q>::Triangle() {
			p1 = glm::vec<2, T, Q>((T)0, (T)0);;
			p2 = glm::vec<2, T, Q>((T)0, (T)0);;
			p3 = glm::vec<2, T, Q>((T)0, (T)0);;
		}

		template<typename T, glm::qualifier Q>
		inline Triangle<T, Q>::Triangle(glm::vec<2, T, Q> p1, glm::vec<2, T, Q> p2, glm::vec<2, T, Q> p3) {
			this->p1 = p1;
			this->p2 = p2;
			this->p3 = p3;
		}

		template<typename T, glm::qualifier Q>
		inline glm::vec<2, T, Q> Triangle<T, Q>::center() {
			return glm::vec<2, T, Q>(
				(p1.x + p2.x + p3.x) / (T)3,
				(p1.y + p2.y + p3.y) / (T)3
			);
		}

		template<typename T, glm::qualifier Q>
		inline Triangle<T, Q> Triangle<T, Q>::empty() {
			return Triangle<T, Q>(
				glm::vec<2, T, Q>((T)0, (T)0),
				glm::vec<2, T, Q>((T)0, (T)0),
				glm::vec<2, T, Q>((T)0, (T)0)
			);
		}

		template<typename T, glm::qualifier Q>
		inline T Triangle<T, Q>::perimeter() {
			return glm::distance(p1, p2) + glm::distance(p2, p3) + glm::distance(p3, p1);
		}

		template<typename T, glm::qualifier Q>
		inline T Triangle<T, Q>::area() {
			return glm::abs((p1.x * (p2.y - p3.y) +
				p2.x * (p3.y - p1.y) +
				p3.x * (p1.y - p2.y)) / 2);
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