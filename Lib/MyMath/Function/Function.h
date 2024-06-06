#pragma once
#include <d3d12.h>

#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>

#include "Vector.h"
#include "Matrix4x4.h"
#include "ComPtr.h"

// Transform構造体
struct Transform {

	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

// 頂点データ構造体
struct VertexData {

	Vector4 pos;
	Vector2 texcoord;
	Vector3 normal;
};

// Material構造体
struct Material {

	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

// Matrix構造体
struct TransformationMatrix {

	Matrix4x4 WVP;
	Matrix4x4 World;
};

// ライト構造体
struct DirectionalLight {

	Vector4 color;     // ライトの色
	Vector3 direction; // ライトの向き
	float intensity;   // 輝度
};

// マテリアルデータ構造体
struct MaterialData {

	std::string textureFilePath;
};

// モデルデータ構造体
struct ModelData {

	std::vector<VertexData> vertices;
	MaterialData material;
};

// カメラビューデータ構造体
struct CameraViewData {

	Vector3 pos;
	float padding;
};

// CBマテリアルデータ
struct CBMaterialData {

	// 頂点マテリアル
	ComPtr<ID3D12Resource> resource;

	// マテリアルマップ
	Material* data = nullptr;
};

// CBトランスフォームデータ
struct CBTransformData {

	// WVP
	ComPtr<ID3D12Resource> resource;

	// 行列
	TransformationMatrix* matrix = nullptr;
};

// CBライトデータ
struct CBLightData {

	// Light
	ComPtr<ID3D12Resource> resource;

	// Light
	DirectionalLight* light = nullptr;
};

// CBカメラビューデータ
struct CBCameraViewData {

	// Camera
	ComPtr<ID3D12Resource> resource;

	// Camera
	CameraViewData* camera = nullptr;
};

// CBデータ
struct CBufferData {

	std::unique_ptr<CBMaterialData> material;
	std::unique_ptr<CBTransformData> matrix;
	std::unique_ptr<CBLightData> light;
	std::unique_ptr<CBCameraViewData> camera;
};

// 三角形の頂点から法線を取得
Vector3 CalculateTriangleNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2);

// πの取得
float GetPI();

// 長さ、ノルム
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

// 内積
float Dot(Vector3 v1, Vector3 v2);

// ベクトル間の距離計算
float Vector3Distance(const Vector3& v1, const Vector3& v2);

// 線形補間関数
float Lerp(float a, float b, float t);

// Vector3線形補間関数
Vector3 Vector3Lerp(const Vector3& a, const Vector3& b, float t);

// イージング アウトサイン
float EaseOutSine(float T);

// 円周上の座標を計算して返す関数
Vector3 CalculateCirclePoint(float radius, int index, int numPoints);

// 球面上の座標を計算して返す関数
Vector3 CalculateSpherePoint(float radius, int index, int numPoints);

// 範囲指定ランダム生成関数
float RandomRange(float min, float max);