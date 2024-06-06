#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <assert.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Camera.h"
#include "Function.h"
#include "ComPtr.h"

//================================================
// ModelManager Class
//================================================
class ModelManager {
public:
	//====================
	// public
	//====================

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	void LoadModel(const std::string& identifier, const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directorypath, const std::string& filename);
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	static ModelManager* Instance();

	// getter

	ModelData GetModelData(const std::string& identifier);

private:

	std::unordered_map<std::string, ModelData> models_;

	ModelManager() = default;
	~ModelManager() = default;

	// コピー禁止
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;
};