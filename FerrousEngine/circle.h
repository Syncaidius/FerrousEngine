#pragma once
#include "shapes_setup.h"
#include <gtc/constants.hpp>

namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Circle {
			T radius;
			glm::vec<2, T, Q> origin;

			Circle() {
				radius = (T)0;
				origin = glm::vec<2, T, Q>((T)0, (T)0);
			}

			Circle(const glm::vec<2, T, Q>& origin, const T radius) {
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
				return 2.0* glm::pi<T>()* radius;
			}

			double diameter() {
				return radius * 2.0;
			}

			double area() {
				return glm::pi<T>()* (radius * radius);
			}


			static Circle<T,Q> empty() {
				return Circle(glm::vec<2, T, Q>((T)0, (T)0, (T)0));
			}
		};

#pragma region OPERATORS
		template<typename T, glm::qualifier Q>
		inline bool operator ==(const Circle<T, Q>& l, const Circle<T, Q>& r) {
			return l.origin == r.origin && l.radius == r.radius;
		}
#pragma endregion

#pragma region CIRCLE TYPES
		/*16-bit signed integer circle.*/
		typedef Circle<int16_t, glm::highp> Circle16;

		/*32-bit signed integer circle.*/
		typedef Circle<int32_t, glm::highp> Circle32;

		/*64-bit integer circle.*/
		typedef Circle<int64_t, glm::highp> Circle64;

		/*16-bit unsigned integer circle.*/
		typedef Circle<uint16_t, glm::highp> CircleU16;

		/*32-bit unsigned integer circle.*/
		typedef Circle<uint32_t, glm::highp> UCircle32;

		/*64-bit unsigned integer circle.*/
		typedef Circle<uint64_t, glm::highp> UCircle64;

		/*32-bit single-precision floating-point circle.*/
		typedef Circle<float, glm::defaultp> CircleF;

		/*64-bit double-precision, floating-point circle.*/
		typedef Circle<double, glm::highp> CircleD;
#pragma endregion
	}
}