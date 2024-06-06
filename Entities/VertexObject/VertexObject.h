#pragma once
#include <Windows.h>

#include <dxgidebug.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <dxcapi.h>

#include "WinApp.h"
#include "Vector.h"
#include "Matrix4x4.h"
#include "Matrix3x3.h"
#include "Function.h"

#include "ComPtr.h"

//================================================
// VertexObject class
//================================================
class VertexObject {
public:
	//====================
	// public
	//====================

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	void CreateTriangleVertexResource(Matrix4x4 wvpMatrix);
	void CreateSpriteVertexResource(Matrix4x4 wvpMatrix);
	void CreateSphereVertexResource(Matrix4x4 wvpMatrix,Matrix4x4 worldMatrix);
	void CreateLightResource(Vector3 lightDirection);
	static VertexObject& Instance();

	// getter

	/*=========================================================================*/
	// Triangle
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }

	ID3D12Resource* GetMaterialResource()const { return materialResource_.Get(); }
	ID3D12Resource* GetWvpResource()const { return wvpResource_.Get(); }

	/*=========================================================================*/
	// Sprite
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferViewSprite() { return vertexBufferViewSprite_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferViewSprite() { return indexBufferViewSprite_; }

	ID3D12Resource* GetMaterialResourceSprite()const { return materialResourceSprite_.Get(); }
	ID3D12Resource* GetTransformationMatrixResourceSprite()const { return transformationMatrixResourceSprite_.Get(); }

	/*=========================================================================*/
	// Sphere
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferViewSphere() { return vertexBufferViewSphere_; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferViewSphere() { return indexBufferViewSphere_; }

	ID3D12Resource* GetMaterialResourceSphere()const { return materialResourceSphere_.Get(); }
	ID3D12Resource* GetWvpResourceSphere()const { return wvpResourceSphere_.Get(); }

	/*=========================================================================*/
	// Ligth
	D3D12_VERTEX_BUFFER_VIEW& GetLightBufferView() { return lightBufferView_; }

	ID3D12Resource* GetLightResource()const { return lightResource_.Get(); }

private:
	//====================
	// private
	//====================

	/*=========================================================================*/
	// UvTransform
	Transform uvTransformSprite_{

		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Matrix4x4 uvTransformMatrix_;

	/*=========================================================================*/
	// Triangle
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ComPtr<ID3D12Resource> vertexResource_;
	ComPtr<ID3D12Resource> materialResource_;
	ComPtr<ID3D12Resource> wvpResource_;

	/*=========================================================================*/
	// Sprite
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	ComPtr<ID3D12Resource> indexResourceSprite_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};
	ComPtr<ID3D12Resource> vertexResourceSprite_;
	ComPtr<ID3D12Resource> materialResourceSprite_;
	ComPtr<ID3D12Resource> transformationMatrixResourceSprite_;

	/*=========================================================================*/
	// Sphere
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere_{};
	ComPtr<ID3D12Resource> indexResourceSphere_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere_{};
	ComPtr<ID3D12Resource> vertexResourceSphere_;
	ComPtr<ID3D12Resource> materialResourceSphere_;
	ComPtr<ID3D12Resource> wvpResourceSphere_;

	/*=========================================================================*/
	// Light
	D3D12_VERTEX_BUFFER_VIEW lightBufferView_;
	ComPtr<ID3D12Resource> lightResource_;
};