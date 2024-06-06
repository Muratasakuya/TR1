#include "Function.h"

// 三角形の頂点から法線を取得
Vector3 CalculateTriangleNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2) {

	// 頂点間のベクトルを計算
	Vector3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	Vector3 vec2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

	// 外積を計算して法線ベクトルを得る
	Vector3 normal;
	normal.x = vec1.y * vec2.z - vec1.z * vec2.y;
	normal.y = vec1.z * vec2.x - vec1.x * vec2.z;
	normal.z = vec1.x * vec2.y - vec1.y * vec2.x;

	// 法線ベクトルを正規化する
	float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;

	return normal;
}

// πの取得
float GetPI() { return (float)M_PI; }

// 長さ、ノルム
float Length(const Vector3& v) {

	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// 正規化
Vector3 Normalize(const Vector3& v) {

	float length = Length(v);
	if (length != 0) {
		return Vector3(v.x / length, v.y / length, v.z / length);
	} else {

		// 値が入ってなければnullで返す
		return Vector3(0.0f, 0.0f, 0.0f);
	}
}

// 内積
float Dot(Vector3 v1, Vector3 v2) {

	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// ベクトル間の距離計算
float Vector3Distance(const Vector3& v1, const Vector3& v2) {

	float dx = v2.x - v1.x;
	float dy = v2.y - v1.y;
	float dz = v2.z - v1.z;

	return std::sqrtf(dx * dx + dy * dy + dz * dz);
}

// 線形補間関数
float Lerp(float a, float b, float t) { return a + (b - a) * t; }

// Vector3線形補間関数
Vector3 Vector3Lerp(const Vector3& a, const Vector3& b, float t) { return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t) }; }

// イージング アウトサイン
float EaseOutSine(float T) {

	return sinf((T * static_cast<float>(M_PI)) / 2.0f);
}

// 円周上の座標を計算して返す関数
Vector3 CalculateCirclePoint(float radius, int index, int numPoints) {

	float angleIncrement = 2.0f * GetPI() / numPoints; // 円周を等分する角度の増加量
	float angle = index * angleIncrement; // この要素に対する角度の計算

	// 極座標から直交座標への変換
	float x = radius * cosf(angle);
	float y = radius * sinf(angle);

	return { x, y, -10.0f };
}

// 球面上の座標を計算して返す関数
Vector3 CalculateSpherePoint(float radius, int index, int numPoints) {
	// 乱数生成を使用して球面上の点を分布させる
	float phi = acosf(1 - 2 * (index + 0.5f) / numPoints); // 0からπまで
	float theta = sqrtf(numPoints * GetPI()) * phi; // 0から2πまで

	// 球座標から直交座標への変換
	float x = radius * sinf(phi) * cosf(theta);
	float y = radius * sinf(phi) * sinf(theta);
	float z = radius * cosf(phi);

	return { x, y, z };
}
// 範囲指定ランダム生成関数
float RandomRange(float min, float max){

	std::mt19937 rng_(std::random_device{}());
	std::uniform_real_distribution<float> uniformDist_(0.0f, 1.0f);

	return min + (max - min) * uniformDist_(rng_);
}
