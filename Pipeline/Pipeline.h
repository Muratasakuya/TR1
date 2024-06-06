#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <memory>
#include <array>
#include <assert.h>

#include "Logger.h"
#include "ComPtr.h"

// パイプラインの種類
enum class PipelineType {

	PRIMITIVE,    // 単色 テクスチャを使用しない
	TEXTURE,     // テクスチャを使う
	BLINNPHONG,  // BlinnPhong反射 テクスチャ付き
};

// パイプライン
struct PipelineObject {

	// ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;
	// パイプラインステート
	ComPtr<ID3D12PipelineState> pipelineState;
};

// パイプラインの種類の数
static inline const uint32_t pipelineNum = 3;

//================================================
// Pipeline Class
//================================================
/// PSO
class Pipeline {
public:
	//====================
	// public
	//====================

	// パイプライン各種生成
	void CreateGraphicsPipelines();

	// getter

	const std::array<std::unique_ptr<PipelineObject>, pipelineNum>& GetPipelines() const { return pipelines_; }

private:
	//====================
	// private
	//====================

	// 各パイプライン
	std::array<std::unique_ptr<PipelineObject>, pipelineNum> pipelines_;

	// パイプライン生成
	std::unique_ptr<PipelineObject> CreatePrimitiveGraphicsPipeline();
	std::unique_ptr<PipelineObject> CreateTextureGraphicsPipeline();
	std::unique_ptr<PipelineObject> CreateBlinnPhongGraphicsPipeline();

	// シェーダのコンパイル
	ComPtr<IDxcBlob> CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);
};