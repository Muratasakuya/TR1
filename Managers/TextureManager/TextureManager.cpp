#include "TextureManager.h"

#include "DirectXCommon.h"
#include "Logger.h"

//============================================================
// シングルトンインスタンス
//============================================================
TextureManager* TextureManager::Instance() {
	static TextureManager instance;
	return &instance;
}

//============================================================
// DescriptorHandleCPUの生成
//============================================================
D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);

	return handleCPU;
}

//============================================================
// DescriptorHandleGPUの生成
//============================================================
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);

	return handleGPU;
}

//============================================================
// Textureデータの読み込み
//============================================================
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {

	// テクスチャファイルを呼んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成 → 元画像よりも小さなテクスチャ群
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

//============================================================
// TextureResourceを作成する関数
//============================================================
ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {

	// metadataを元にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);                             // Textureの幅
	resourceDesc.Height = UINT(metadata.height);                           // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);                   // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);            // 奥行き　or 配列textureの配列数
	resourceDesc.Format = metadata.format;                                 // TextureのFormat
	resourceDesc.SampleDesc.Count = 1;                                     // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数、普段は2次元

	// 利用するHeapの設定、非常に特殊な運用。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;                        // 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;          // プロセッサの近くに配置

	// Resourceの作成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr =
		device->CreateCommittedResource(
			&heapProperties,                   // Heapの設定
			D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定
			&resourceDesc,                     // Resourceの設定
			D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState、Textureは基本読むだけ
			nullptr,                           // Clear最適地、使わない。
			IID_PPV_ARGS(&resource)            // 作成するResourceポインタへのポインタ
		);
	assert(SUCCEEDED(hr));

	return resource;
}

//============================================================
// TextureResourceにデータを転送する関数
//============================================================
void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {

		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,              // 全領域へのコピー
			img->pixels,          // 元データアドレス
			UINT(img->rowPitch),  // 1ラインサイズ
			UINT(img->slicePitch) // 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

//============================================================
//  動的なテクスチャ読み込み関数
//============================================================
void TextureManager::LoadTexture(const std::string& identifier, const std::string& filePath) {

	DirectXCommon* dxCommon = DirectXCommon::Instance();

	// テクスチャを読み込む
	DirectX::ScratchImage mipImages = LoadTexture(filePath);
	DirectX::TexMetadata metadata = mipImages.GetMetadata();
	ComPtr<ID3D12Resource> textureResource = CreateTextureResource(dxCommon->GetDevice(), metadata);
	UploadTextureData(textureResource.Get(), mipImages);

	// ハンドルを生成
	UINT descriptorSize = dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize, static_cast<uint32_t>(textures_.size()));
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize, static_cast<uint32_t>(textures_.size()));

	// SRVを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, cpuHandle);

	// テクスチャデータをマップに格納
	textures_[identifier] = { textureResource, cpuHandle, gpuHandle, metadata };
}

//============================================================
// Shader Resource Viewの生成
//============================================================
void TextureManager::Initialize() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	// Descriptor Heapを作成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 4;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	directXCommon->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap_));
}

//============================================================
// SRVのセット
//============================================================
void TextureManager::SetGraphicsRootDescriptorTable(
	ID3D12GraphicsCommandList* commandList, UINT rootParamaterIndex, std::string identifier) {

	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootDescriptorTable(rootParamaterIndex, textures_[identifier].gpuHandle);
}