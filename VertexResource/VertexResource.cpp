#include "VertexResource.h"

#include "DirectXCommon.h"

//============================================================
// シングルトンインスタンス
//============================================================
VertexResource* VertexResource::Instance() {
	static VertexResource instance;
	return &instance;
}

//============================================================
// BufferResourceの作成
//============================================================
ComPtr<ID3D12Resource> VertexResource::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {

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
// マテリアル生成
//============================================================
std::unique_ptr<CBMaterialData> VertexResource::CreateMaterial() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	HRESULT hr;
	std::unique_ptr<CBMaterialData> material = std::make_unique<CBMaterialData>();

	// 頂点マテリアルの生成
	material->resource = CreateBufferResource(directXCommon->GetDevice(), sizeof(Material));

	// マテリアルデータのマッピング
	hr = material->resource->Map(0, nullptr, reinterpret_cast<void**>(&material->data));

	// 作れなければエラー
	assert(SUCCEEDED(hr));

	return material;
}

//============================================================
// WVP生成
//============================================================
std::unique_ptr<CBTransformData> VertexResource::CreateWVP() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	HRESULT hr;
	std::unique_ptr<CBTransformData> matrix = std::make_unique<CBTransformData>();

	// WVPの生成
	matrix->resource = CreateBufferResource(directXCommon->GetDevice(), sizeof(TransformationMatrix));

	// WVPデータのマッピング
	hr = matrix->resource->Map(0, nullptr, reinterpret_cast<void**>(&matrix->matrix));

	// 作れなければエラー
	assert(SUCCEEDED(hr));

	return matrix;
}

//============================================================
// Light生成
//============================================================
std::unique_ptr<CBLightData> VertexResource::CreateLight() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	HRESULT hr;
	std::unique_ptr<CBLightData> light = std::make_unique<CBLightData>();

	// Lightの生成
	light->resource = CreateBufferResource(directXCommon->GetDevice(), sizeof(DirectionalLight));

	// Lightデータのマッピング
	hr = light->resource->Map(0, nullptr, reinterpret_cast<void**>(&light->light));

	// 作れなければエラー
	assert(SUCCEEDED(hr));

	return light;
}

//============================================================
// CameraView生成
//============================================================
std::unique_ptr<CBCameraViewData> VertexResource::CreateCameraView() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	HRESULT hr;
	std::unique_ptr<CBCameraViewData> camera = std::make_unique<CBCameraViewData>();

	// CameraViewの生成
	camera->resource = CreateBufferResource(directXCommon->GetDevice(), sizeof(CameraViewData));

	// CameraViewデータのマッピング
	hr = camera->resource->Map(0, nullptr, reinterpret_cast<void**>(&camera->camera));

	// 作れなければエラー
	assert(SUCCEEDED(hr));

	return camera;
}