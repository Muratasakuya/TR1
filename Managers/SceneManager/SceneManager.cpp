#include "SceneManager.h"

//============================================================
// コンストラクタ
//============================================================
SceneManager::SceneManager() {

	// 各シーン生成
	sceneArr_[GAME] = std::make_unique<GameScene>();
}

//============================================================
// デストラクタ
//============================================================
SceneManager::~SceneManager() {

	// 解放
	sceneArr_[GAME].reset();
}

//============================================================
// 各シーンの初期化 更新　描画
//============================================================
void SceneManager::Run() {

	// ウィンドウのxボタンが押されるまでループ
	while (Engine::ProcessMessage() == 0) {
		// フレームの開始
		Engine::BeginFrame();

		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();

		// 初期化
		if (prevSceneNo_ != currentSceneNo_) {

			sceneArr_[currentSceneNo_]->Initialize();
		}

		// 更新 描画
		sceneArr_[currentSceneNo_]->Update();
		sceneArr_[currentSceneNo_]->Draw();

		// フレームの終了
		Engine::EndFrame();
	}
}