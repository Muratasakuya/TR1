//#include "Sprite.h"
//
////============================================================
//// 初期化
////============================================================
//void Sprite::Initialize() {
//
//	// SRT
//	scale_ = { 1.0f,1.0f,1.0f };
//	rotate_ = { 0.0f,0.0f,0.0f };
//	translate_ = { 0.0f,0.0f,0.0f };
//
//	// Matrix
//	worldMatrix_ =
//		MakeAffineMatrix(scale_, rotate_, translate_);
//	viewMatrix_ = MakeIdentity4x4();
//	projectionMatrix_= MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
//	wvpMatrix_ = Multiply(worldMatrix_, Multiply(viewMatrix_, projectionMatrix_));
//}
//
////============================================================
//// 更新処理
////============================================================
//void Sprite::Update() {
//
//	// 行列の更新処理
//	worldMatrix_ =
//		MakeAffineMatrix(scale_, rotate_, translate_);
//	wvpMatrix_ = Multiply(worldMatrix_, Multiply(viewMatrix_, projectionMatrix_));
//}
//
////============================================================
//// 描画処理
////============================================================
//void Sprite::Draw() {
//
//	System::DrawSprite(wvpMatrix_);
//}