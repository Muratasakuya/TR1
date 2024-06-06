#pragma once
#include "Engine.h"
#include "Vector.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Function.h"

#include "VertexResource.h"

#include <string>

//================================================
// Teapot Class
//================================================
class Teapot {
public:
	//====================
	// public
	//====================

	// コンストラクタ
	Teapot();

	// デストラクタ
	~Teapot();

	// 初期化
	void Initialize(Camera* camera);

	// 更新
	void Update(Camera* camera);

	// 描画
	void Draw();

	// getter


	// setter



private:
	//====================
	// private
	//====================

	// バッファデータ
	std::unique_ptr<CBufferData> cBuffer_ = nullptr;

	TransformationMatrix matrix_;

	Transform transform_;

	Vector3 lightDirection_;

	// modelの種類
	std::string modelName_;

	// パイプラインタイプ
	PipelineType pipelineType_;

};

