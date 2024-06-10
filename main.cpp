#include "Engine.h"

#include "SceneManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// メインシステムの初期化
	Engine::Initialize(1280, 720);

	// シーン管理クラスのインスタンス
	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();

	// メインループ
	sceneManager->Run();

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}