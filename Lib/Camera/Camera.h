#pragma once
#include "Vector.h"
#include "Matrix4x4.h"

//================================================
// Camera Class
//================================================
class Camera{
public:
	//====================
	// public
	//====================

	void Initialize();
	void Update();
	void ImGuiDraw();

	static Camera& Instance();

	// getter

	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix()const { return projectionMatrix_; }
	Vector3 GetTranslate()const { return translate_; }

private:
	//====================
	// private
	//====================

	Matrix4x4 cameraMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 orthoMatrix_;
	Matrix4x4 projectionMatrix_;

	Vector3 scale_;
	Vector3 rotate_;
	Vector3 translate_;

	// 透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	// 正射影行列
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	// ビューポート変換行列
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
};