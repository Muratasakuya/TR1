#pragma once
#include "Engine.h"
#include "Vector.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Function.h"

#include "VertexResource.h"

#include <string>

//================================================
// Triangle Class
//================================================
class Triangle {
public:
	//====================
	// public
	//====================

	// コンストラクタ
	Triangle();

	// デストラクタ
	~Triangle();

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

	// textureの種類
	std::string textureName_;

	//  パイプラインタイプ
	PipelineType pipelineType_;
};