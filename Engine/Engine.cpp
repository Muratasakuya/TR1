#include "Engine.h"

#include <objbase.h>
#include <array>
#include <memory>
#include <utility>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "VertexObject.h"

//============================================================
// namespace
//============================================================
namespace {

	//============================================================
	// EngineSystem class 
	//============================================================
	class EngineSystem {
		friend class Engine;
	public:
		//====================
		// public
		//====================

		EngineSystem() = default;
		~EngineSystem() = default;

		// 初期化
		void Initialize();

		// リセット
		void Reset();

		// システムの終了
		void Finalize();

	private:
		//====================
		// private
		//====================

		/*-----------------------------------------------------------------------------------------*/
		/// シングルトンインスタンス変数



		WinApp* winApp_ = nullptr;
		DirectXCommon* directXCommon_ = nullptr;
		ImGuiManager* imgui_ = nullptr;
		TextureManager* textureManager_ = nullptr;
		ModelManager* modelManager_ = nullptr;



		/*-----------------------------------------------------------------------------------------*/
		/// 頂点情報



		// 三角形の最大数
		static const int32_t kMaxTriangleNum = 3;
		// 三角形の頂点数
		static const UINT kTriangleVertexNum = 3;
		// 三角形使用数
		uint32_t indexTriangle = 0;

		// 三角錐の最大数
		static const int32_t kMaxTriangularPrismNum = 1;
		// 三角錐の頂点数
		static const UINT kTriangularPrismVertexNum = 12;
		// 三角錐使用数
		uint32_t indexTriangularPrism = 0;



		/*-----------------------------------------------------------------------------------------*/
		/// パイプライン



		std::unique_ptr<Pipeline> pipeline_;



		/*-----------------------------------------------------------------------------------------*/
		/// メッシュ



#pragma region /// 三角メッシュ ///
		// メッシュデータ
		struct MeshData {

			// 頂点バッファ
			ComPtr<ID3D12Resource> vertexResource;
			// 頂点バッファビュー
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

			// 頂点バッファデータ
			VertexData* vertex = nullptr;
		};
		// 三角形メッシュデータ
		std::unique_ptr<MeshData> triangle_;
		// メッシュ生成
		std::unique_ptr<MeshData> CreateMesh(UINT vertexCount);
#pragma endregion

#pragma region /// 三角錐メッシュ ///
		// 三角錐データ
		struct TriangularPrismData {

			// 頂点バッファ
			ComPtr<ID3D12Resource> vertexResource;
			// 頂点バッファビュー
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

			// 頂点バッファデータ
			VertexData* vertex = nullptr;
		};
		// 三角錐データ
		std::unique_ptr<TriangularPrismData> triangularPrism_;
		// 三角錐メッシュの生成
		std::unique_ptr<TriangularPrismData> CreateTriangularPrismMesh(UINT vertexCount);
#pragma endregion

#pragma region /// モデルメッシュ ///
		// モデルデータ
		struct ModelMeshData {

			// 頂点バッファ
			ComPtr<ID3D12Resource> vertexResource;
			// 頂点バッファビュー
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

			// 頂点バッファデータ
			VertexData* vertex = nullptr;
		};
		// モデルデータ
		std::unordered_map<std::string, std::unique_ptr<ModelMeshData>> models_;
		// モデルメッシュの生成
		std::unique_ptr<ModelMeshData> CreateModelMesh(UINT vertexCount);
		// 各種モデルの生成
		void CreateModel(const std::string& identifier);

		// 可変引数設定
		template <typename... Args>
		void CreateModels(Args&&... args) {
			(CreateModel(std::forward<Args>(args)), ...);
		}
#pragma endregion

		// メッシュ生成
		void CreateMeshes();



		/*-----------------------------------------------------------------------------------------*/
		/// 描画関数



		// 三角形描画
		void DrawTriangle(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);

		// 三角錐
		void DrawTriangularPrism(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);

		// モデル
		void DrawModel(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);


		/*-----------------------------------------------------------------------------------------*/
		/// その他、生成を行う関数



		// BufferResource生成
		ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);



		/*-----------------------------------------------------------------------------------------*/
		/// システム



		// フレーム開始処理
		void BeginFrame();

		// フレーム終了処理
		void EndFrame();

		// メッセージの受け渡し処理
		bool ProcessMessage();



		/*-----------------------------------------------------------------------------------------*/

		// コピー禁止
		EngineSystem(const EngineSystem&) = delete;
		EngineSystem& operator=(const EngineSystem&) = delete;
	};

#pragma region // システム //

	//============================================================
	// 初期化
	//============================================================
	void EngineSystem::Initialize() {

		// シングルトンインスタンスの代入
		directXCommon_ = DirectXCommon::Instance();
		winApp_ = WinApp::Instance();
		imgui_ = ImGuiManager::Instance();
		textureManager_ = TextureManager::Instance();
		modelManager_ = ModelManager::Instance();

		// 画像の読み込み
		textureManager_->LoadTexture("uvCheckerTexture", "./Resources/Images/uvChecker.png");
		textureManager_->LoadTexture("whiteTexture", "./Resources/Images/whiteTexture.png");
		textureManager_->LoadTexture("monsterBallTexture", "./Resources/Images/monsterBall.png");
		textureManager_->LoadTexture("marioTexture", "./Resources/Images/mario.png");

		// モデルの読み込み
		/*modelManager_->LoadModel("bunny", "./Resources/Obj", "bunny.obj");
		modelManager_->LoadModel("plane", "./Resources/Obj", "plane.obj");
		modelManager_->LoadModel("teapot", "./Resources/Obj", "teapot.obj");
		modelManager_->LoadModel("suzanne", "./Resources/Obj", "suzanne.obj");*/

		// パイプライン各種生成
		pipeline_ = std::make_unique<Pipeline>();
		pipeline_->CreateGraphicsPipelines();

		// 三角形メッシュ生成
		CreateMeshes();
		// モデル生成
		//CreateModels("bunny", "plane", "teapot", "suzanne");
	}

	//============================================================
	// リセット
	//============================================================
	void EngineSystem::Reset() {

		// 0に戻す
		indexTriangle = 0;
		indexTriangularPrism = 0;
	}

	//============================================================
	// フレーム開始処理
	//============================================================
	void EngineSystem::BeginFrame() {

		imgui_->Begin();
		directXCommon_->PreDraw();
	}

	//============================================================
	// フレーム終了処理
	//============================================================
	void EngineSystem::EndFrame() {

		imgui_->End();
		imgui_->Draw();
		directXCommon_->PostDraw();

		Reset();
	}

	//============================================================
	// メッセージの受け渡し処理
	//============================================================
	bool EngineSystem::ProcessMessage() {

		if (winApp_->ProcessMessage()) {
			return 1;
		} else {
			return 0;
		}
	}

	//============================================================
	// システム終了
	//============================================================
	void EngineSystem::Finalize() {

		pipeline_.reset();
		triangle_.reset();
	}

#pragma endregion

#pragma region // 生成 //

	//============================================================
	// BufferResourceの作成
	//============================================================
	ComPtr<ID3D12Resource> EngineSystem::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

		HRESULT hr;

		// 頂点リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		// 頂点リソースの設定
		D3D12_RESOURCE_DESC vertexResourceDesc{};
		// バッファリソース。テクスチャの場合はまた別の設定をする
		vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		// リソースのサイズ、今回はVector4を3頂点分
		vertexResourceDesc.Width = sizeInBytes;
		// バッファの場合はこれらは1にする決まり
		vertexResourceDesc.Height = 1;
		vertexResourceDesc.DepthOrArraySize = 1;
		vertexResourceDesc.MipLevels = 1;
		vertexResourceDesc.SampleDesc.Count = 1;
		// バッファの場合はこれにする決まり
		vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		// 実際に頂点リソースを作る
		ComPtr<ID3D12Resource> bufferResource = nullptr;
		hr = device->CreateCommittedResource(
			&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
		assert(SUCCEEDED(hr));

		return bufferResource;
	}

	//============================================================
	// メッシュの生成
	//============================================================
	std::unique_ptr<EngineSystem::MeshData> EngineSystem::CreateMesh(UINT vertexCount) {

		HRESULT hr;
		std::unique_ptr<MeshData> mesh = std::make_unique<MeshData>();

		if (vertexCount) {

			// 頂点データサイズ
			UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertexCount);

			// 頂点バッファの生成
			mesh->vertexResource = CreateBufferResource(directXCommon_->GetDevice(), sizeVB);

			// 頂点バッファビューの作成
			mesh->vertexBufferView.BufferLocation = mesh->vertexResource->GetGPUVirtualAddress();
			mesh->vertexBufferView.SizeInBytes = sizeVB;
			mesh->vertexBufferView.StrideInBytes = sizeof(VertexData);

			// 頂点データのマッピング
			hr = mesh->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mesh->vertex));
			assert(SUCCEEDED(hr));
		}

		return mesh;
	}

	//============================================================
	// 三角錐メッシュの生成
	//============================================================
	std::unique_ptr<EngineSystem::TriangularPrismData> EngineSystem::CreateTriangularPrismMesh(UINT vertexCount) {

		HRESULT hr;
		std::unique_ptr<TriangularPrismData> triangular = std::make_unique<TriangularPrismData>();

		if (vertexCount) {

			// 頂点データサイズ
			UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertexCount);

			// 頂点バッファの生成
			triangular->vertexResource = CreateBufferResource(directXCommon_->GetDevice(), sizeVB);

			// 頂点バッファビューの作成
			triangular->vertexBufferView.BufferLocation = triangular->vertexResource->GetGPUVirtualAddress();
			triangular->vertexBufferView.SizeInBytes = sizeVB;
			triangular->vertexBufferView.StrideInBytes = sizeof(VertexData);

			// 頂点データのマッピング
			hr = triangular->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&triangular->vertex));
			assert(SUCCEEDED(hr));
		}

		return triangular;
	}

	//============================================================
	// モデルメッシュの生成
	//============================================================
	std::unique_ptr<EngineSystem::ModelMeshData> EngineSystem::CreateModelMesh(UINT vertexCount) {

		HRESULT hr;
		std::unique_ptr<ModelMeshData> model = std::make_unique<ModelMeshData>();

		if (vertexCount) {

			// 頂点データサイズ
			UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertexCount);

			// 頂点バッファの生成
			model->vertexResource = CreateBufferResource(directXCommon_->GetDevice(), sizeVB);

			// 頂点バッファビューの作成
			model->vertexBufferView.BufferLocation = model->vertexResource->GetGPUVirtualAddress();
			model->vertexBufferView.SizeInBytes = sizeVB;
			model->vertexBufferView.StrideInBytes = sizeof(VertexData);

			// 頂点データのマッピング
			hr = model->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&model->vertex));
			assert(SUCCEEDED(hr));
		}

		return model;
	}

	//============================================================
	// 各種モデルの生成
	//============================================================
	void EngineSystem::CreateModel(const std::string& identifier) {

		const auto& modelData = modelManager_->GetModelData(identifier);
		models_[identifier] = CreateModelMesh(static_cast<UINT>(modelData.vertices.size()));
	}

	//============================================================
	// 各メッシュデータの生成
	//============================================================
	void EngineSystem::CreateMeshes() {

		UINT triangleVertexCount = kMaxTriangleNum * kTriangleVertexNum;

		// 三角形
		triangle_ = CreateMesh(triangleVertexCount);

		UINT triangulerPrismVertexCount = kMaxTriangularPrismNum * kTriangularPrismVertexNum;

		// 三角錐
		triangularPrism_ = CreateTriangularPrismMesh(triangulerPrismVertexCount);
	}

#pragma endregion

#pragma region // 描画 //

	//============================================================
	// 三角形の描画
	//============================================================
	void EngineSystem::DrawTriangle(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

		// CommandListをdxCommonClassからもってくる
		ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon_->GetCommandList();

		// 法線の計算
		Vector3 normal =
			CalculateTriangleNormal({ -1.0f,-1.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f,1.0f }, { 1.0f,-1.0f,0.0f,1.0f });

		// 頂点データ
		std::array vertices = {

			VertexData{{-1.0f,-1.0f,0.0f,1.0f},{ 0.0f,1.0f },normal},
			VertexData{{0.0f,1.0f,0.0f,1.0f},{ 0.5f,0.0f },normal},
			VertexData{{1.0f,-1.0f,0.0f,1.0f},{ 1.0f,1.0f },normal}
		};

		// 三角形使用数に応じた頂点数の計算
		size_t indexVertex = indexTriangle * kTriangleVertexNum;

		assert(vertices.size() <= kTriangleVertexNum);

		// 頂点バッファへデータ転送
		std::memcpy(&triangle_->vertex[indexVertex], vertices.data(), sizeof(vertices[0]) * vertices.size());

		// RootSignatureの設定
		commandList->SetGraphicsRootSignature(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->rootSignature.Get());
		// PipelineStateの設定
		commandList->SetPipelineState(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->pipelineState.Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &triangle_->vertexBufferView);

		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, cBufferData->material->resource->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, cBufferData->matrix->resource->GetGPUVirtualAddress());

		if (PipelineType::TEXTURE == pipelineType || PipelineType::BLINNPHONG == pipelineType) {

			// Light用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, cBufferData->light->resource->GetGPUVirtualAddress());

			// SRVのセット
			textureManager_->SetGraphicsRootDescriptorTable(commandList.Get(), 2, identifier);
		}

		// 描画を行う(DrawCall)。3頂点で1つのインスタンス
		commandList->DrawInstanced(kTriangleVertexNum, 1, static_cast<INT>(indexVertex), 0);

		// 使用カウント上昇
		indexTriangle++;
	}

	//============================================================
	// 三角錐の描画
	//============================================================
	void EngineSystem::DrawTriangularPrism(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

		// CommandListをdxCommonClassからもってくる
		ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon_->GetCommandList();

		// 頂点データ
		std::array vertices = {

			// 底面の正三角形
			VertexData{{-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f})},
			VertexData{{0.5f, -0.2887f, 0.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f})},
			VertexData{{0.0f, 0.5774f, 0.0f, 1.0f}, {0.5f, 0.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f})},

			// 面1
		   VertexData{{-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{0.0f, 0.5774f, 0.0f, 1.0f}, {0.5f, 0.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},

		   // 面2
		   VertexData{{0.0f, 0.5774f, 0.0f, 1.0f}, {0.5f, 0.0f}, CalculateTriangleNormal({0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{0.5f, -0.2887f, 0.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.5774f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},

		   // 面3
		   VertexData{{0.5f, -0.2887f, 0.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{-0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})},
		   VertexData{{0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, CalculateTriangleNormal({-0.5f, -0.2887f, 0.0f, 1.0f}, {0.5f, -0.2887f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f})}
		};

		// 三角形使用数に応じた頂点数の計算
		size_t indexVertex = indexTriangularPrism * kTriangularPrismVertexNum;

		assert(vertices.size() <= kTriangularPrismVertexNum);

		// 頂点バッファへデータ転送
		std::memcpy(&triangularPrism_->vertex[indexVertex], vertices.data(), sizeof(vertices[0]) * vertices.size());

		// RootSignatureの設定
		commandList->SetGraphicsRootSignature(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->rootSignature.Get());
		// PipelineStateの設定
		commandList->SetPipelineState(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->pipelineState.Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &triangularPrism_->vertexBufferView);

		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, cBufferData->material->resource->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, cBufferData->matrix->resource->GetGPUVirtualAddress());

		if (PipelineType::TEXTURE == pipelineType || PipelineType::BLINNPHONG == pipelineType) {

			// Light用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, cBufferData->light->resource->GetGPUVirtualAddress());

			// SRVのセット
			textureManager_->SetGraphicsRootDescriptorTable(commandList.Get(), 2, identifier);
		}

		// 描画を行う(DrawCall)。4頂点で1つのインスタンス
		commandList->DrawInstanced(kTriangularPrismVertexNum, 1, static_cast<INT>(indexVertex), 0);

		// 使用カウント上昇
		indexTriangularPrism++;
	}

	//============================================================
	// モデルの描画
	//============================================================
	void EngineSystem::DrawModel(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

		// CommandListをdxCommonClassからもってくる
		ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon_->GetCommandList();

		// 頂点バッファへデータ転送
		std::memcpy(models_[identifier]->vertex, modelManager_->GetModelData(identifier).vertices.data(), sizeof(VertexData) * modelManager_->GetModelData(identifier).vertices.size());

		// RootSignatureの設定
		commandList->SetGraphicsRootSignature(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->rootSignature.Get());
		// PipelineStateの設定
		commandList->SetPipelineState(pipeline_->GetPipelines()[static_cast<size_t>(pipelineType)]->pipelineState.Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// 頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &models_[identifier]->vertexBufferView);
		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, cBufferData->material->resource->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, cBufferData->matrix->resource->GetGPUVirtualAddress());
		// Camera用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, cBufferData->camera->resource->GetGPUVirtualAddress());


		// Light用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(3, cBufferData->light->resource->GetGPUVirtualAddress());

		if (identifier != "suzanne") {

			// SRVのセット
			textureManager_->SetGraphicsRootDescriptorTable(commandList.Get(), 2, "uvCheckerTexture");
		}

		// 描画を行う(DrawCall)。modelData.vertices.size()で1つのインスタンス
		commandList->DrawInstanced(UINT(modelManager_->GetModelData(identifier).vertices.size()), 1, 0, 0);
	}

#pragma endregion

	/*--------------------------------------------------------------------------------------------------*/
	/// ↓ Engine classで使う ↓

	WinApp* swinApp = nullptr;
	DirectXCommon* sdirectXCommon = nullptr;
	ImGuiManager* simgui = nullptr;
	TextureManager* stexture = nullptr;
	std::unique_ptr<EngineSystem> sEngineSystem = nullptr;
}

//============================================================
// 初期化
//============================================================
void Engine::Initialize(int width, int height) {

	// ComInitialize
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// シングルトンインスタンスの代入
	swinApp = WinApp::Instance();
	sdirectXCommon = DirectXCommon::Instance();
	simgui = ImGuiManager::Instance();
	stexture = TextureManager::Instance();

	// エラー、警告が出たら止める
	sdirectXCommon->DebugLayer();
	// ウィンドウの生成
	swinApp->CreateMainWindow(width, height);

	// DirectXの初期化
	sdirectXCommon->Initialize(swinApp, width, height);

	// ImGuiの初期化
	simgui->Initialize();

	// Texture
	stexture->Initialize();

	// EngineSystem
	sEngineSystem = std::make_unique<EngineSystem>();
	sEngineSystem->Initialize();
}

//============================================================
// フレーム開始処理
//============================================================
void Engine::BeginFrame() { sEngineSystem->BeginFrame(); }

//============================================================
// フレーム終了処理
//============================================================
void Engine::EndFrame() { sEngineSystem->EndFrame(); }

//============================================================
// メッセージの受け渡し処理
//============================================================
bool Engine::ProcessMessage() { return sEngineSystem->ProcessMessage(); }

//============================================================
// 終了処理
//============================================================
void Engine::Finalize() {

	sEngineSystem->Finalize();
	sEngineSystem.reset();
	simgui->Finalize();
	sdirectXCommon->Finalize(swinApp);

	// ComFinalize
	CoUninitialize();
}

//============================================================
// 描画処理
//============================================================
// 三角形
void Engine::DrawTriangle(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

	sEngineSystem->DrawTriangle(identifier, cBufferData, pipelineType);
}

// 三角錐
void Engine::DrawTriangularPrism(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

	sEngineSystem->DrawTriangularPrism(identifier, cBufferData, pipelineType);
}

// モデル
void Engine::DrawModel(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType) {

	sEngineSystem->DrawModel(identifier, cBufferData, pipelineType);
}