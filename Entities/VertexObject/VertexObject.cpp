#include "VertexObject.h"

#include "DirectXCommon.h"
#include "Function.h"

// lib /* .hに書いてはいけない */
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>


//============================================================
// シングルトンインスタンス
//============================================================
VertexObject& VertexObject::Instance() {
	static VertexObject instance;
	return instance;
}

//============================================================
// BufferResourceの作成
//============================================================
ComPtr<ID3D12Resource> VertexObject::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

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
// TriangleVertexResourceの生成
//============================================================
void VertexObject::CreateTriangleVertexResource(Matrix4x4 wvpMatrix) {

	DirectXCommon* directXCommon = nullptr;
	directXCommon = DirectXCommon::Instance();

	///////////////////////////////////////////////////////////
	// BufferResourceの作成
	///////////////////////////////////////////////////////////

	vertexResource_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 6);
	materialResource_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));

	// WVP用のリソースを作る、Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));

	///////////////////////////////////////////////////////////
	// VertexBufferViewの生成
	///////////////////////////////////////////////////////////

	// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	///////////////////////////////////////////////////////////
	// Resourceにデータを書き込む
	///////////////////////////////////////////////////////////

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 左下1
	vertexData[0].pos = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	// 上1
	vertexData[1].pos = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	// 右下1
	vertexData[2].pos = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	// 左下2
	vertexData[3].pos = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	// 上2
	vertexData[4].pos = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	// 右下2
	vertexData[5].pos = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 書き込むためのアドレスを取得
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	// Lightingを無効にする
	materialData->enableLighting = false;

	// 単位行列で初期化
	materialData->uvTransform = MakeIdentity4x4();

	// wvpにデータを書き込む
	TransformationMatrix* matrix = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&matrix));
	// wvp行列を書き込んでおく
	matrix->WVP = wvpMatrix;
}

//============================================================
// SpriteVertexResourceの生成
//============================================================
void VertexObject::CreateSpriteVertexResource(Matrix4x4 wvpMatrix) {

	DirectXCommon* directXCommon = nullptr;
	directXCommon = DirectXCommon::Instance();

	///////////////////////////////////////////////////////////
	// BufferResourceの作成
	///////////////////////////////////////////////////////////

	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 4);
	// Sprite用のTransformationMatrix用のリソースを作る、Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));

	// マテリアル用
	materialResourceSprite_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));

	// index
	indexResourceSprite_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * 6);

	///////////////////////////////////////////////////////////
	// VertexBufferViewの生成
	///////////////////////////////////////////////////////////

	// リソースの先頭アドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	// index
	// リソースの先頭アドレスから使う
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tにする
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;

	///////////////////////////////////////////////////////////
	// Resourceにデータを書き込む
	///////////////////////////////////////////////////////////

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	// 1=3、2=5、頂点4つで描画
	// 左下
	vertexDataSprite[0].pos = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	// 左上
	vertexDataSprite[1].pos = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
	// 右下
	vertexDataSprite[2].pos = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
	// 右上
	vertexDataSprite[3].pos = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	// データを書き込む
	TransformationMatrix* matrix = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&matrix));
	// wvp行列を書き込んでおく
	matrix->WVP = wvpMatrix;

	/*============================================================================================================*/
	// ImGui、UvMatrixの計算

	// ImGui
	ImGui::Begin("SpriteUvTransform");

	ImGui::DragFloat2("UVTranslate", &uvTransformSprite_.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite_.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite_.rotate.z);

	ImGui::End();

	// Affine
	uvTransformMatrix_ =
		MakeAffineMatrix(uvTransformSprite_.scale, uvTransformSprite_.rotate, uvTransformSprite_.translate);

	/*============================================================================================================*/

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	// 色
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	// Lightingを無効にする
	materialData->enableLighting = false;
	// 単位行列で初期化
	materialData->uvTransform = uvTransformMatrix_;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;
}

//============================================================
// SphereVertexResourceの生成
//============================================================
void VertexObject::CreateSphereVertexResource(Matrix4x4 wvpMatrix, Matrix4x4 worldMatrix) {

	DirectXCommon* directXCommon = nullptr;
	directXCommon = DirectXCommon::Instance();

	///////////////////////////////////////////////////////////
	// BufferResourceの作成
	///////////////////////////////////////////////////////////

	// Sphere用のリソースを作る
	vertexResourceSphere_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(VertexData) * 1024);

	// WVP用のリソースを作る、Matrix4x4 1つ分のサイズを用意する
	wvpResourceSphere_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));

	// マテリアル用
	materialResourceSphere_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));

	// index
	indexResourceSphere_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(uint32_t) * 1536);

	///////////////////////////////////////////////////////////
	// VertexBufferViewの生成
	///////////////////////////////////////////////////////////

	// リソースの先頭アドレスから使う
	vertexBufferViewSphere_.BufferLocation = vertexResourceSphere_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSphere_.SizeInBytes = sizeof(VertexData) * 1024;
	// 1頂点あたりのサイズ
	vertexBufferViewSphere_.StrideInBytes = sizeof(VertexData);

	// index
	// リソースの先頭アドレスから使う
	indexBufferViewSphere_.BufferLocation = indexResourceSphere_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス1536分のサイズ
	indexBufferViewSphere_.SizeInBytes = sizeof(uint32_t) * 1536;
	// インデックスはuint32_tにする
	indexBufferViewSphere_.Format = DXGI_FORMAT_R32_UINT;

	///////////////////////////////////////////////////////////
	// Resourceにデータを書き込む
	///////////////////////////////////////////////////////////

	VertexData* vertexDataSphere = nullptr;
	vertexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));

	// 分割数
	const uint32_t kSubdivision = 16;

	// 経度分割1つ分の角度
	const float kLonEvery = GetPI() * 2.0f / float(kSubdivision);

	// 緯度分割1つ分の角度
	const float kLatEvery = GetPI() / float(kSubdivision);

	// 緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {

		float lat = -GetPI() / 2.0f + kLatEvery * latIndex;

		// 経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;

			float u0 = static_cast<float>(lonIndex) / kSubdivision;
			float u1 = static_cast<float>(lonIndex + 1) / kSubdivision;
			float v0 = 1.0f - static_cast<float>(latIndex) / kSubdivision;
			float v1 = 1.0f - static_cast<float>(latIndex + 1) / kSubdivision;

			/*=========================================================================================*/
			// 各頂点の値を書き込む
			// 基準点 a
			vertexDataSphere[start].pos.x =
				std::cos(lat) * std::cos(lon);

			vertexDataSphere[start].pos.y =
				std::sin(lat);

			vertexDataSphere[start].pos.z =
				std::cos(lat) * std::sin(lon);

			vertexDataSphere[start].pos.w = 1.0f;

			vertexDataSphere[start].texcoord = { u0, v0 };

			vertexDataSphere[start].normal.x = vertexDataSphere[start].pos.x;
			vertexDataSphere[start].normal.y = vertexDataSphere[start].pos.y;
			vertexDataSphere[start].normal.z = vertexDataSphere[start].pos.z;

			/*=========================================================================================*/
			// 基準点 b
			vertexDataSphere[start + 1].pos.x =
				std::cos(lat + kLatEvery) * std::cos(lon);

			vertexDataSphere[start + 1].pos.y =
				std::sin(lat + kLatEvery);

			vertexDataSphere[start + 1].pos.z =
				std::cos(lat + kLatEvery) * std::sin(lon);

			vertexDataSphere[start + 1].pos.w = 1.0f;

			vertexDataSphere[start + 1].texcoord = { u0, v1 };

			vertexDataSphere[start + 1].normal.x = vertexDataSphere[start + 1].pos.x;
			vertexDataSphere[start + 1].normal.y = vertexDataSphere[start + 1].pos.y;
			vertexDataSphere[start + 1].normal.z = vertexDataSphere[start + 1].pos.z;

			/*=========================================================================================*/
			// 基準点 c
			vertexDataSphere[start + 2].pos.x =
				std::cos(lat) * std::cos(lon + kLonEvery);

			vertexDataSphere[start + 2].pos.y =
				std::sin(lat);

			vertexDataSphere[start + 2].pos.z =
				std::cos(lat) * std::sin(lon + kLonEvery);

			vertexDataSphere[start + 2].pos.w = 1.0f;

			vertexDataSphere[start + 2].texcoord = { u1, v0 };

			vertexDataSphere[start + 2].normal.x = vertexDataSphere[start + 2].pos.x;
			vertexDataSphere[start + 2].normal.y = vertexDataSphere[start + 2].pos.y;
			vertexDataSphere[start + 2].normal.z = vertexDataSphere[start + 2].pos.z;

			/*=========================================================================================*/
			// 基準点 d
			vertexDataSphere[start + 3].pos.x =
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);

			vertexDataSphere[start + 3].pos.y =
				std::sin(lat + kLatEvery);

			vertexDataSphere[start + 3].pos.z =
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);

			vertexDataSphere[start + 3].pos.w = 1.0f;

			vertexDataSphere[start + 3].texcoord = { u1, v1 };

			vertexDataSphere[start + 3].normal.x = vertexDataSphere[start + 3].pos.x;
			vertexDataSphere[start + 3].normal.y = vertexDataSphere[start + 3].pos.y;
			vertexDataSphere[start + 3].normal.z = vertexDataSphere[start + 3].pos.z;
		}
	}

	/*=========================================================================================================
	*
	*							↑　本来ここもクラス化して座標を入れるべき ↑
	*
	==========================================================================================================*/

	// wvpにデータを書き込む
	TransformationMatrix* matrix = nullptr;
	// 書き込むためのアドレスを取得
	wvpResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&matrix));
	// wvp,world行列を書き込んでおく
	matrix->WVP = wvpMatrix;
	matrix->World = worldMatrix;

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 書き込むためのアドレスを取得
	materialData->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	// Lightingを有効にする
	materialData->enableLighting = true;

	// 単位行列で初期化
	materialData->uvTransform = MakeIdentity4x4();

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSphere = nullptr;
	// 書き込むためのアドレスを取得
	indexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSphere));

	// Indexの計算
	for (uint32_t latIndex = 0; latIndex <= kSubdivision; latIndex++) {
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; lonIndex++) {
			uint32_t baseIndex = (latIndex * (kSubdivision)+lonIndex) * 4;

			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex)] = baseIndex;         // 0
			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex) + 1] = baseIndex + 1; // 1
			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex) + 2] = baseIndex + 2; // 2
			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex) + 3] = baseIndex + 2; // 3
			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex) + 4] = baseIndex + 1; // 4
			indexDataSphere[6 * (latIndex * kSubdivision + lonIndex) + 5] = baseIndex + 3; // 5
		}
	}
}

//============================================================
// LightResourceの生成
//============================================================
void VertexObject::CreateLightResource(Vector3 lightDirection) {

	DirectXCommon* directXCommon = nullptr;
	directXCommon = DirectXCommon::Instance();

	///////////////////////////////////////////////////////////
	// BufferResourceの作成
	///////////////////////////////////////////////////////////

	// Light用のリソースを作る
	lightResource_ = CreateBufferResource(directXCommon->GetDevice(), sizeof(DirectionalLight));

	///////////////////////////////////////////////////////////
	// VertexBufferViewの生成
	///////////////////////////////////////////////////////////

	// リソースの先頭アドレスから使う
	lightBufferView_.BufferLocation = lightResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点
	lightBufferView_.SizeInBytes = sizeof(DirectionalLight);
	// 1頂点あたりのサイズ
	lightBufferView_.StrideInBytes = sizeof(DirectionalLight);

	///////////////////////////////////////////////////////////
	// Resourceにデータを書き込む
	///////////////////////////////////////////////////////////

	// directionalLightにデータを書き込む
	DirectionalLight* directionalLight = nullptr;
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight));

	// 必要な値を書き込む
	directionalLight->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight->direction = lightDirection;
	directionalLight->intensity = 1.0f;
}