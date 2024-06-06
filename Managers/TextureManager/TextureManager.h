#pragma once
#include <DirectXTex.h>
#include <d3dx12.h>

#include <unordered_map>
#include <string>
#include <vector>

#include "ComPtr.h"

//================================================
// TextrueManager Class
//================================================
class TextureManager {
public:
	//====================
	// public
	//====================

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	void Initialize();

	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParamaterIndex, std::string identifier);

	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	void LoadTexture(const std::string& identifier, const std::string& filePath);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static TextureManager* Instance();

private:
	//====================
	// privete
	//====================

	// テクスチャデータ
	struct TextureData {

		ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		DirectX::TexMetadata metadata;
	};

	std::unordered_map<std::string, TextureData> textures_;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	TextureManager() = default;
	~TextureManager() = default;

	// コピー禁止
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
};