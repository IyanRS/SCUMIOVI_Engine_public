#pragma once

#include <cmath>
#include <type_traits>
#include <string>

template<typename T = float>
class Vector2 {
	static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "T must be a number");
public:
	Vector2(T x = 0, T y = 0) : x(x), y(y) {}
	T x;
	T y;
	float distanceTo(Vector2<T> targetVector) {
		return static_cast<float>(std::sqrt(std::pow(x - targetVector.x, 2) + std::pow(y - targetVector.y, 2)));
	}
	Vector2<float> normalized() {
		float length = distanceTo(Vector2()); // Distance to (0,0)
		return Vector2(x / length, y / length);
	}
	Vector2<T> operator-(const Vector2<T>& v2) const { return Vector2(x - v2.x, y - v2.y); }
	Vector2<T> operator+(const Vector2<T>& v2) const { return Vector2(x + v2.x, y + v2.y); }
	Vector2<T>& operator+=(const Vector2<T>& v2) { x += v2.x; y += v2.y; return *this; }
	Vector2<T> operator*(float value) const { return Vector2(x*value, y*value); }
	Vector2<float> operator/(const float value) const { return Vector2(x / value, y / value); }
	bool operator==(const Vector2<T>& v2) const { return x == v2.x && y == v2.y; }
	bool operator!=(const Vector2<T>& v2) const { return x != v2.x || y != v2.y; }
	float dot(Vector2<T> other) { return x * other.x + y * other.y; };

	std::string toString() { 
		return std::to_string(x) + "," + std::to_string(y);
	};

	Vector2& operator=(const Vector2<T>& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
		}
		return *this;
	}


};

// Hash function for Vector2<T>
struct Vector2Hash {
	template <typename T>
	std::size_t operator()(const Vector2<T>& v) const {
		std::size_t h1 = std::hash<T>()(v.x);
		std::size_t h2 = std::hash<T>()(v.y);
		return h1 ^ (h2 << 1);  // Combine the two hashes (using bitwise XOR for better distribution)
	}
};