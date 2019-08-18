#pragma once
#include <glm.hpp>

namespace fe {
	namespace shapes {
		template<typename T, glm::qualifier Q>
		struct Line {
			glm::vec<2, T, Q> start;
			glm::vec<2, T, Q> end;

			inline Line() {
				start = glm::vec<2, T, Q>((T)0, (T)0);
				end = start;
			}

			Line(glm::vec<2, T, Q> start, glm::vec<2, T, Q> end) {
				start = start;
				end = end;
			}

			Line(T startX, T startY, T endX, T endY) {
				start = glm::vec<2, T, Q>(startX, startY);
				end = glm::vec<2, T, Q>(endX, endY);
			}

			T length() {
				return glm::distance(start, end);
			}

			glm::vec<2, T, Q> center() {
				return glm::mix(start, end, 0.5);
			}

			/* Gets a point along the line, based on a percentage of the line's total length from the start point.*/
			glm::vec<2, T, Q> pointOn(double distPercent) {
				return glm::mix(start, end, distPercent);
			}

			static Line<T, Q> empty() {
				return glm::distance(start, end);
			}

			/* Creates a line starting from a point of origin toward  the given direction. */
			static Line<T, Q> toDirection(glm::vec<2, T, Q> origin, glm::vec<2, T, Q> direction, T distance) {
				return Line<T, Q>(origin, origin + (direction * distance));
			}
		};

#pragma region OPERATORS
		template<typename T, glm::qualifier Q>
		inline bool operator ==(const Line<T, Q>& l, const Line<T, Q>& r) {
			return l.start == r.start && l.end == r.end;
		}
#pragma endregion

#pragma region LINE TYPES
		/*16-bit signed integer line.*/
		typedef Line<int16_t, glm::highp> Line16;

		/*32-bit signed integer line.*/
		typedef Line<int32_t, glm::highp> Line32;

		/*64-bit integer line.*/
		typedef Line<int64_t, glm::highp> Line64;

		/*16-bit unsigned integer line.*/
		typedef Line<uint16_t, glm::highp> LineU16;

		/*32-bit unsigned integer line.*/
		typedef Line<uint32_t, glm::highp> ULine32;

		/*64-bit unsigned integer line.*/
		typedef Line<uint64_t, glm::highp> ULine64;

		/*32-bit single-precision floating-point line.*/
		typedef Line<float, glm::defaultp> LineF;

		/*64-bit double-precision, floating-point line.*/
		typedef Line<double, glm::highp> LineD;
#pragma endregion	
	}
}