#include "GameScene.h"

#include "ImGuiManager.h"

//============================================================
// コンストラクタ
//============================================================
GameScene::GameScene() {

	/*======================================================*/
	// カメラ

	// 生成
	camera_ = std::make_unique<Camera>();

	// 初期化
	camera_->Initialize();

	/*======================================================*/
	// 三角形

	// 生成
	triangle_ = std::make_unique<Triangle>();

	// 初期化
	triangle_->Initialize(camera_.get());

	/*======================================================*/
	// 3Dモデル

	/*-------------------------------------*/
	// bunny
	// 生成
	bunny_ = std::make_unique<Bunny>();

	// 初期化
	bunny_->Initialize(camera_.get());

	/*-------------------------------------*/
	// plane
	// 生成
	plane_ = std::make_unique<Plane>();

	// 初期化
	plane_->Initialize(camera_.get());

	/*-------------------------------------*/
	// teapot
	// 生成
	teapot_ = std::make_unique<Teapot>();

	// 初期化
	teapot_->Initialize(camera_.get());

	/*-------------------------------------*/
	// suzanne
	// 生成
	suzanne_ = std::make_unique<Suzanne>();

	// 初期化
	suzanne_->Initialize(camera_.get());
}

//============================================================
// デストラクタ
//============================================================
GameScene::~GameScene() {

	// 解放
	camera_.reset();
	triangle_.reset();
	bunny_.reset();
	plane_.reset();
	teapot_.reset();
}

//============================================================
// 初期化
//============================================================
void GameScene::Initialize() {

	/*======================================================*/
	// カメラ

	camera_->Initialize();

	/*======================================================*/
	// 三角形

	triangle_->Initialize(camera_.get());

	/*======================================================*/
	// 3Dモデル

	// bunny
	bunny_->Initialize(camera_.get());

	// plane
	plane_->Initialize(camera_.get());

	// teapot
	teapot_->Initialize(camera_.get());

	// suzanne
	suzanne_->Initialize(camera_.get());
}

//============================================================
// 更新処理
//============================================================
void GameScene::Update() {

	/*======================================================*/
	// カメラ

	camera_->Update();

	/*======================================================*/
	// 三角形

	//triangle_->Update(camera_.get());

	/*======================================================*/
	// 3Dモデル

	// bunny
	bunny_->Update(camera_.get());

	// plane
	//plane_->Update(camera_.get());

	// teapot
	teapot_->Update(camera_.get());

	// suzanne
	suzanne_->Update(camera_.get());

}

//============================================================
// 描画処理
//============================================================
void GameScene::Draw() {

	camera_->ImGuiDraw();

	/*======================================================*/
	// 三角形

	//triangle_->Draw();
	
	/*======================================================*/
	// 3Dモデル

	// bunny
	bunny_->Draw();

	// plane
	//plane_->Draw();

	// teapot
	teapot_->Draw();

	// suzanne
	suzanne_->Draw();

}