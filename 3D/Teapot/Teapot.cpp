#include "Teapot.h"

#include "ModelManager.h"
#include "ImGuiManager.h"

//============================================================
// コンストラクタ
//============================================================
Teapot::Teapot() {

	// CBufferの作成
	cBuffer_ = std::make_unique<CBufferData>();
	cBuffer_->material = VertexResource::Instance()->CreateMaterial();
	cBuffer_->matrix = VertexResource::Instance()->CreateWVP();
	cBuffer_->light = VertexResource::Instance()->CreateLight();
	cBuffer_->camera = VertexResource::Instance()->CreateCameraView();
}

//============================================================
// デストラクタ
//============================================================
Teapot::~Teapot() {

	cBuffer_->material.reset();
	cBuffer_->matrix.reset();
	cBuffer_->light.reset();
	cBuffer_->camera.reset();
	cBuffer_.reset();
}

//============================================================
// 更新処理
//============================================================
void Teapot::Initialize(Camera* camera) {

	// model
	// identifier
	modelName_ = "teapot";

	// パイプラインのタイプ
	pipelineType_ = PipelineType::BLINNPHONG;

	// SRT
	transform_.scale = { 0.5f,0.5f,0.5f };
	transform_.rotate = { 0.0f,0.0f,0.0f };
	transform_.translate = { -0.13f,0.99f,0.0f };

	// Matrix
	matrix_.World =
		MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matrix_.WVP = Multiply(matrix_.World, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

	// Light
	lightDirection_ = { 0.0f,0.0f,1.0f };

	/*================================================================================================================*/
	// CBufferの設定

	// Material
	cBuffer_->material->data->color = { 1.0f,1.0f,1.0f,1.0f };
	//Lighting
	cBuffer_->material->data->enableLighting = true;
	cBuffer_->material->data->uvTransform = MakeIdentity4x4();
	// Matrix
	cBuffer_->matrix->matrix->World = matrix_.World;
	cBuffer_->matrix->matrix->WVP = matrix_.WVP;
	// Light
	cBuffer_->light->light->color = { 1.0f,1.0f,1.0f,1.0f };
	cBuffer_->light->light->direction = lightDirection_;
	cBuffer_->light->light->intensity = 1.0f;
	// Camera
	cBuffer_->camera->camera->pos = camera->GetTranslate();
}


//============================================================
// 更新処理
//============================================================
void Teapot::Update(Camera* camera) {

	/*----------------------------------------------------------------------------------------------------------------*/
	/// ImGui

	ImGui::Begin("Teapot");


	ImGui::SliderFloat3("scale", &transform_.scale.x, 0.0f, 1.0f);
	ImGui::SliderAngle("rotateX", &transform_.rotate.x);
	ImGui::SliderAngle("rotateY", &transform_.rotate.y);
	ImGui::SliderAngle("rotateZ", &transform_.rotate.z);
	ImGui::SliderFloat3("translate", &transform_.translate.x, -5.0f, 5.0f);
	ImGui::SliderFloat3("LightDirection", &lightDirection_.x, -1.0f, 1.0f);

	ImGui::End();

	/*----------------------------------------------------------------------------------------------------------------*/

	// Matrix
	matrix_.World =
		MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	matrix_.WVP = Multiply(matrix_.World, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

	// CBufferの更新
	// Matrix
	cBuffer_->matrix->matrix->World = matrix_.World;
	cBuffer_->matrix->matrix->WVP = matrix_.WVP;
	// Light
	cBuffer_->light->light->direction = lightDirection_;
	// Camera
	cBuffer_->camera->camera->pos = camera->GetTranslate();
}

//============================================================
// 描画処理
//============================================================
void Teapot::Draw() {

	// モデルの描画

	// teapot
	Engine::DrawModel(modelName_, cBuffer_.get(), pipelineType_);
}