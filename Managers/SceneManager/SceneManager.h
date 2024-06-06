#pragma once
#include "Engine.h"

/// Base
#include "IScene.h"

/// from IScene
#include "GameScene.h"

#include <memory>

//================================================
// SceneManager Class
//================================================
class SceneManager {
public:
	//====================
	// public
	//====================

	// コンストラクタ
	SceneManager();

	// デストラクタ
	~SceneManager();

	// 全体ループ
	void Run();

private:
	//====================
	// private
	//====================

	std::unique_ptr<IScene> sceneArr_[1];

	SceneNo currentSceneNo_{};
	SceneNo prevSceneNo_{};

};

