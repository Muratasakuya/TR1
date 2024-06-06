#pragma once
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

//================================================
// 2次元ベクトル
//================================================
struct Vector2 {

	float x, y;
};

//================================================
// 3次元ベクトル
//================================================
struct Vector3 {

	float x, y, z;

	// 算術演算子
	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}
	Vector3 operator*(float scalar) const {
		return Vector3(x * scalar, y * scalar, z * scalar);
	}
	friend Vector3 operator*(float scalar, const Vector3& v) {
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}
	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& operator-=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
};

//================================================
// 4次元ベクトル
//================================================
struct Vector4 {

	float x, y, z, w;
};