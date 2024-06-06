#include "Triangle.h"

#include "ImGuiManager.h"

//============================================================
// コンストラクタ
//============================================================
Triangle::Triangle() {

	// CBufferの作成
	cBuffer_ = std::make_unique<CBufferData>();
	cBuffer_->material = VertexResource::Instance()->CreateMaterial();
	cBuffer_->matrix = VertexResource::Instance()->CreateWVP();
	cBuffer_->light = VertexResource::Instance()->CreateLight();
}

//============================================================
// デストラクタ
//============================================================
Triangle::~Triangle() {

	cBuffer_->material.reset();
	cBuffer_->matrix.reset();
	cBuffer_.reset();
}

//============================================================
// 初期化
//============================================================
void Triangle::Initialize(Camera* camera) {

	// Texture
	// identifier
	textureName_ = "uvCheckerTexture";

	// パイプラインのタイプ
	pipelineType_ = PipelineType::PRIMITIVE;

	// SRT
	transform_.scale = { 1.0f,1.0f,1.0f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { 0.0,0.0f,0.0f };

	// Matrix
	matrix_.World =
		MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matrix_.WVP = Multiply(matrix_.World, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

	/*================================================================================================================*/
	// CBufferの設定

	// Material
	cBuffer_->material->data->color = { 1.0f,1.0f,1.0f,1.0f };
	//Lighting無効
	cBuffer_->material->data->enableLighting = false;
	cBuffer_->material->data->uvTransform = MakeIdentity4x4();
	// Matrix
	cBuffer_->matrix->matrix->World = matrix_.World;
	cBuffer_->matrix->matrix->WVP = matrix_.WVP;
	// Light
	cBuffer_->light->light->color = { 1.0f,1.0f,1.0f,1.0f };
	cBuffer_->light->light->direction = { 0.0f,-1.0f,0.0f };
	cBuffer_->light->light->intensity = 1.0f;
}

//============================================================
// 更新
//============================================================
void Triangle::Update(Camera* camera) {

	// Matrix
	matrix_.World =
		MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matrix_.WVP = Multiply(matrix_.World, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

	// CBufferの更新
	// Matrix
	cBuffer_->matrix->matrix->World = matrix_.World;
	cBuffer_->matrix->matrix->WVP = matrix_.WVP;
}

//============================================================
// 描画
//============================================================
void Triangle::Draw() {

	// 三角形の描画
	Engine::DrawTriangle(textureName_, cBuffer_.get(), pipelineType_);
}