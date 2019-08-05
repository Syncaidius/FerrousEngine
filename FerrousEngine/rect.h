#pragma once
#include <glm.hpp>

template<typename T, glm::qualifier Q>
struct Rect {
	T left;
	T top;
	T right;
	T bottom;

	Rect(T left, T top, T right, T bottom);
	
	glm::vec<2, T, Q> topLeft();
	glm::vec<2, T, Q> topRight();
	glm::vec<2, T, Q> bottomLeft();
	glm::vec<2, T, Q> bottomRight();
	glm::vec<2, T, Q> center();

	T width();
	T height();

	void inflate(T horizontal, T vertical);
	inline void inflate(T amount) { inflate(amount, amount); }
	Rect<T, Q> getInflated(T horizontal, T vertical);
	inline Rect<T, Q> getInflated(T amount) { return getInflated(amount, amount); }

	bool contains(T x, T y);
	bool contains(const glm::vec<2, T, Q>& v);
	bool contains(const Rect<T, Q>& rect);
	bool intersects(const Rect<T, Q>& v);
	Rect<T, Q> getIntersection(const Rect<T, Q>& other);

	/* Creates a new rectangle that exactly contains two other rectangles. */
	Rect<T, Q> unify(const Rect<T, Q>& a, const Rect<T, Q>& b);
};

#pragma region IMPLEMENTATION
template<typename T, glm::qualifier Q>
Rect<T, Q>::Rect(T left, T top, T right, T bottom) {
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

template<typename T, glm::qualifier Q>
inline glm::vec<2, T, Q> Rect<T, Q>::topLeft() {
	return glm::vec<2, T, Q>(left, top);
}

template<typename T, glm::qualifier Q>
inline glm::vec<2, T, Q> Rect<T, Q>::topRight() {
	return glm::vec<2, T, Q>(right, top);
}

template<typename T, glm::qualifier Q>
inline glm::vec<2, T, Q> Rect<T, Q>::bottomLeft() {
	return glm::vec<2, T, Q>(left, bottom);
}

template<typename T, glm::qualifier Q>
inline glm::vec<2, T, Q> Rect<T, Q>::bottomRight() {
	return glm::vec<2, T, Q>(right, bottom);
}

template<typename T, glm::qualifier Q>
inline glm::vec<2, T, Q> Rect<T, Q>::center() {
	return glm::vec<2, T, Q>(
		left + (right - left) / static_cast<T>(2),
		top + (bottom - top) / static_cast<T>(2));
}

template<typename T, glm::qualifier Q>
inline T Rect<T, Q>::width() {
	return right - left;
}

template<typename T, glm::qualifier Q>
inline T Rect<T, Q>::height() {
	return bottom - top;
}

template<typename T, glm::qualifier Q>
inline void Rect<T, Q>::inflate(T horizontal, T vertical) {
	left -= horizontal;
	top -= vertical;
	right == horizontal;
	bottom += vertical;
}

template<typename T, glm::qualifier Q>
inline Rect<T, Q> Rect<T, Q>::getInflated(T horizontal, T vertical) {
	return Rect<T, Q>(
		left - horizontal,
		top - vertical,
		right + horizontal,
		bottom + vertical);
}

template<typename T, glm::qualifier Q>
inline bool Rect<T, Q>::contains(T x, T y) {
	return (x >= left) && (x <= right) && (y >= top) && (y <= bottom);
}

template<typename T, glm::qualifier Q>
inline bool Rect<T, Q>::contains(const glm::vec<2, T, Q>& v) {
	return contains(v.x, v.y);
}

template<typename T, glm::qualifier Q>
inline bool Rect<T, Q>::contains(const Rect<T, Q>& v) {
	return (v.left >= left) && (v.right <= right) && (v.top >= top) && (v.bottom <= bottom);
}

template<typename T, glm::qualifier Q>
inline bool Rect<T, Q>::intersects(const Rect<T, Q>& v) {
	return (v.left < right) && (left < v.right) && (v.top < bottom) && (top < v.bottom);
}

#pragma endregion

#pragma region RECTANGLE TYPES
/*16-bit signed integer rectangle.*/
typedef Rect<int16_t, glm::highp> Rect16;

/*32-bit signed integer rectangle.*/
typedef Rect<int32_t, glm::highp> Rect32;

/*64-bit integer rectangle.*/
typedef Rect<int64_t, glm::highp> Rect64;

/*16-bit unsigned integer rectangle.*/
typedef Rect<uint16_t, glm::highp> RectU16;

/*32-bit unsigned integer rectangle.*/
typedef Rect<uint32_t, glm::highp> URect32;

/*64-bit unsigned integer rectangle.*/
typedef Rect<uint64_t, glm::highp> URect64;

/*32-bit single-precision floating-point rectangle.*/
typedef Rect<float, glm::defaultp> RectF;

/*64-bit double-precision, floating-point rectangle.*/
typedef Rect<double, glm::highp> RectD;
