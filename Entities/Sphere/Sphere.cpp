//#include "Sphere.h"
//
//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx12.h>
//
////============================================================
//// 初期化
////============================================================
//void Sphere::Initialize(Camera* camera) {
//
//	// SRT
//	scale_ = { 1.0f,1.0f,1.0f };
//	rotate_ = { 0.0f,0.0f,0.0f };
//	translate_ = { 0.0f,0.0f,0.0f };
//
//	// Matrix
//	worldMatrix_ =
//		MakeAffineMatrix(scale_, rotate_, translate_);
//	wvpMatrix_ = Multiply(worldMatrix_, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));
//
//	lightDirection_ = { 0.0f,-1.0f,0.0f };
//}
//
////============================================================
//// 更新処理
////============================================================
//void Sphere::Update(Camera* camera) {
//
//	/*===========================================================================================================*/
//	// ImGui
//	ImGui::Begin("Light");
//
//	ImGui::SliderFloat3("lightDirection", &lightDirection_.x, -1.0f, 1.0f);
//
//	ImGui::End();
//
//	// y軸回転
//	rotate_.y += 0.015f;
//
//	// 行列の更新処理
//	worldMatrix_ =
//		MakeAffineMatrix(scale_, rotate_, translate_);
//	wvpMatrix_ = Multiply(worldMatrix_, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));
//}
//
////============================================================
//// 描画処理
////============================================================
//void Sphere::Draw() {
//
//	System::DrawSphere(wvpMatrix_,worldMatrix_,lightDirection_);
//}