#pragma once
#include <assert.h>

#include <d3d12.h>

#include <memory>

#include "Function.h"
#include "ComPtr.h"

//================================================
// VertexResource Class
//================================================
/// CBuffer
class VertexResource{
public:
	//====================
	// public
	//====================

	// マテリアル生成
	std::unique_ptr<CBMaterialData> CreateMaterial();

	// WVP生成
	std::unique_ptr<CBTransformData> CreateWVP();

	// Light生成
	std::unique_ptr<CBLightData> CreateLight();

	// Camera生成
	std::unique_ptr<CBCameraViewData> CreateCameraView();

	// シングルトン
	static VertexResource* Instance();

private:
	//====================
	// peivate
	//====================

	// BufferResource生成
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};