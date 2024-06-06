#pragma once

// シーンの種類
enum SceneNo {

	GAME
};

//================================================
// IScene Class
//================================================
/// Base
class IScene {
public:
	//====================
	// public
	//====================

	// コンストラクタ
	IScene() {}

	// 仮想デストラクタ
	virtual ~IScene() {}

	// 純粋仮想関数
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	/// getter

	// シーンの取得
	SceneNo GetSceneNo() const { return sceneNo_; }

protected:
	//====================
	// protected
	//====================

	// シーン
	static SceneNo sceneNo_;

};