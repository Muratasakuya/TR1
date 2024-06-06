#pragma once

/// Engine
#include "Engine.h"

/// c++
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

/// 3Dオブジェクト
#include "Camera.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Sprite.h"

/// 3Dモデル
#include "Bunny.h"
#include "Plane.h"
#include "Teapot.h"
#include "Suzanne.h"

/// OpenCV
#include "OpenCV.h"

/// Base
#include "IScene.h"

//================================================
// GameScene Class
//================================================
class GameScene :
	public IScene {
public:
	//====================
	// public
	//====================

	// コンストラクタ
	GameScene();

	// デストラクタ
	~GameScene();

	void Initialize()override;
	void Update()override;
	void Draw()override;

private:
	//====================
	// private
	//====================

	/*----------------------------------------------------------------------*/
	// OpenCV
	
	// インスタンス
	std::unique_ptr<OpenCV> openCV_;
};