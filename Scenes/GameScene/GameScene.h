#pragma once

// Engine
#include "Engine.h"

// c++
#include <memory>
#include <vector>
#include <algorithm>
#include <random>

// 3Dオブジェクト
#include "Camera.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Sprite.h"

// 3Dモデル
#include "Bunny.h"
#include "Plane.h"
#include "Teapot.h"
#include "Suzanne.h"

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
	// カメラ

	std::unique_ptr<Camera> camera_;

	/*----------------------------------------------------------------------*/
	// 三角形

	std::unique_ptr<Triangle> triangle_;

	/*----------------------------------------------------------------------*/
	// 3Dモデル

	// bunny
	std::unique_ptr<Bunny> bunny_;

	// plane
	std::unique_ptr<Plane> plane_;

	// teapot
	std::unique_ptr<Teapot> teapot_;

	// suzanne
	std::unique_ptr<Suzanne> suzanne_;

};