#include "Camera.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

//============================================================
// シングルトンインスタンス
//============================================================
Camera& Camera::Instance() {
	static Camera instance;
	return instance;
}

//============================================================
// 透視投影行列
//============================================================
Matrix4x4 Camera::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = 1.0f / (aspectRatio * std::tanf(fovY / 2.0f));
	matrix.m[1][1] = 1.0f / std::tanf(fovY / 2.0f);
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][2] = (-farClip * nearClip) / (farClip - nearClip);

	return matrix;
}

//============================================================
// 正射影行列
//============================================================
Matrix4x4 Camera::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}

//============================================================
// ビューポート変換行列
//============================================================
Matrix4x4 Camera::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 matrix = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m[i][j] = 0.0f;
		}
	}

	matrix.m[0][0] = width / 2.0f;
	matrix.m[1][1] = -height / 2.0f;
	matrix.m[2][2] = maxDepth - minDepth;
	matrix.m[3][0] = left + width / 2.0f;
	matrix.m[3][1] = top + height / 2.0f;
	matrix.m[3][2] = minDepth;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

//============================================================
// 初期化
//============================================================
void Camera::Initialize() {

	// アフィン
	scale_ = { 1.0f,1.0f,1.0f };
	rotate_ = { 0.0f,0.0f,0.0f };
	translate_ = { 0.0f,0.0f,-10.0f };

	cameraMatrix_ =
		MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = Inverse(cameraMatrix_);

	projectionMatrix_ =
		MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
}

//============================================================
// 更新処理
//============================================================
void Camera::Update() {

	cameraMatrix_ =
		MakeAffineMatrix(scale_, rotate_, translate_);
	viewMatrix_ = Inverse(cameraMatrix_);
}

//============================================================
// ImGuiの描画処理
//============================================================
void Camera::ImGuiDraw() {

	ImGui::Begin("Camera");

	ImGui::SliderAngle("rotateX", &rotate_.x);
	ImGui::SliderAngle("rotateY", &rotate_.y);
	ImGui::SliderAngle("rotateZ", &rotate_.z);
	ImGui::SliderFloat3("translate", &translate_.x, -20.0f, 50.0f);

	ImGui::End();
}