#pragma once
#include <glm.hpp>
#include <gtc/constants.hpp>

namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Circle {
			T radius;
			glm::vec<2, T, Q> origin;

			Circle();
			Circle(const glm::vec<2, T, Q>& origin, const T radius);

			void inflate(const T amount);
			Circle<T, Q> getInflated(const T amount);

			double circumference();
			double diameter();
			double area();

			static Circle<T,Q> empty();
		};

#pragma region IMPLEMENTATION
		template<typename T, glm::qualifier Q>
		inline Circle<T, Q>::Circle() {
			radius = (T)0;
			origin = glm::vec<2, T, Q>((T)0, (T)0);
		}

		template<typename T, glm::qualifier Q>
		inline Circle<T, Q>::Circle(const glm::vec<2, T, Q>& origin, const T radius) {
			this->origin = origin;
			this->radius = radius;
		}

		template<typename T, glm::qualifier Q>
		inline double Circle<T, Q>::circumference() {
			return 2.0* glm::pi<T>() * radius;
		}

		template<typename T, glm::qualifier Q>
		inline double Circle<T, Q>::diameter() {
			return radius * 2.0;
		}

		template<typename T, glm::qualifier Q>
		inline double Circle<T, Q>::area() {
			return glm::pi<T>() * (radius * radius);
		}

		template<typename T, glm::qualifier Q>
		inline Circle<T, Q> Circle<T, Q>::empty() {
			return Circle(glm::vec<2, T, Q>((T)0, (T)0, (T)0));
		}

		template<typename T, glm::qualifier Q>
		inline void Circle<T, Q>::inflate(const T amount) {
			radius += amount;
		}

		template<typename T, glm::qualifier Q>
		inline Circle<T, Q> Circle<T, Q>::getInflated(const T amount) {
			return Circle(origin, radius + amount);
		}

#pragma endregion

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