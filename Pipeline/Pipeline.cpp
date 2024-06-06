#include "Pipeline.h"

#include "DirectXCommon.h"

//============================================================
// DXCを使用してShaderをCompileする
//============================================================
ComPtr<IDxcBlob> Pipeline::CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler
) {

	HRESULT hr;

	///////////////////////////////////////////////////////////
	// 1.hlslファイルを読み込む
	///////////////////////////////////////////////////////////

	// ここからシェーダーをコンパイルする旨をログに出す
	Log(ConvertWString(std::format(L"Begin CompilerShader, path:{}. profile:{}\n", filePath, profile)));
	// hlslファイルを読み込む
	IDxcBlobEncoding* shaderSouce = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSouce);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSouce->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSouce->GetBufferSize();
	// UTF8の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	///////////////////////////////////////////////////////////
	// 2.Compileする
	///////////////////////////////////////////////////////////

	LPCWSTR arguments[] = {
		filePath.c_str(),          // コンパイル対象のファイル名
		L"-E",L"main",             // エントリーポイントの指定、基本的にmain以外にはしない
		L"-T",profile,             // ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",   // デバッグ用の情報を埋め込む
		L"-Od",                    // 最適化を外しておく
		L"-Zpr",                   // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,          // 読み込んだファイル
		arguments,                    // コンパイルオプション
		_countof(arguments),          // コンパイルオプションの数
		includeHandler,               // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)   // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	///////////////////////////////////////////////////////////
	// 3.警告、エラーが出ていないか確認する
	///////////////////////////////////////////////////////////

	// 警告、エラーが出たらログに出して止める
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		// 警告、エラー
		assert(false);
	}

	///////////////////////////////////////////////////////////
	// 4.Complie結果を受け取って返す
	///////////////////////////////////////////////////////////

	// コンパイル結果から実行用のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertWString(std::format(L"Complie Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSouce->Release();
	shaderResult->Release();
	shaderError->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}

//============================================================
// プリミティブ適応パイプライン生成
//============================================================
std::unique_ptr<PipelineObject> Pipeline::CreatePrimitiveGraphicsPipeline() {

	DirectXCommon* dxCommon = DirectXCommon::Instance();

	HRESULT hr;

	std::unique_ptr<PipelineObject> pipeline = std::make_unique<PipelineObject>();

	ComPtr<ID3DBlob> errorBlob = nullptr; // エラー
	ComPtr<ID3DBlob> signatureBlob = nullptr;

	ComPtr<IDxcBlob> vsBlob = nullptr; // 頂点シェーダ
	ComPtr<IDxcBlob> psBlob = nullptr; // ピクセルシェーダ

	/// RootSignature
#pragma region /// RootSignature ///

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma endregion

	/// rootParameter
#pragma region /// rootParameter ///

	D3D12_ROOT_PARAMETER TEXTURERootParameters[2] = {};
	TEXTURERootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	TEXTURERootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	TEXTURERootParameters[0].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	TEXTURERootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	TEXTURERootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;   // VertexShaderで使う
	TEXTURERootParameters[1].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	descriptionRootSignature.pParameters = TEXTURERootParameters;                // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(TEXTURERootParameters);    // 配列の長さ

#pragma endregion

	/// InputLayout
#pragma region /// InputLayout ///

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

	/// バイナリをもとに生成
#pragma region /// バイナリをもとに生成 rootSignature　///

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {

		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline->rootSignature));
	assert(SUCCEEDED(hr));

#pragma endregion

	/// BlendState
#pragma region /// BlendState ///

	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	// ブレンドモードNone D3D12_COLOR_WRITE_ENABLE_ALLだけ
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion

	/// RasterizerState
#pragma region /// RasterizerState ///

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計周り)を表示しない、背面カリング
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

	/// ShaderComplie
#pragma region /// ShaderComplie ///

	// 頂点シェーダ
	vsBlob =
		CompileShader(L"./Resources/Shaders/Primitive.VS.hlsl", L"vs_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(vsBlob != nullptr);

	// ピクセルシェーダ
	psBlob =
		CompileShader(L"./Resources/Shaders/Primitive.PS.hlsl", L"ps_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(psBlob != nullptr);

#pragma endregion

	/// DepthStencil
#pragma region /// DepthStencil ///

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depth機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行う
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

	/*========================================================================================================*/
	// Pipeline State Objectの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipeline->rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(),vsBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(),psBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	pipeline->pipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pipeline->pipelineState));
	assert(SUCCEEDED(hr));

	return pipeline;
}

//============================================================
// テクスチャ適応パイプライン生成
//============================================================
std::unique_ptr<PipelineObject> Pipeline::CreateTextureGraphicsPipeline() {

	DirectXCommon* dxCommon = DirectXCommon::Instance();

	HRESULT hr;

	std::unique_ptr<PipelineObject> pipeline = std::make_unique<PipelineObject>();

	ComPtr<ID3DBlob> errorBlob = nullptr; // エラー
	ComPtr<ID3DBlob> signatureBlob = nullptr;

	ComPtr<IDxcBlob> vsBlob = nullptr; // 頂点シェーダ
	ComPtr<IDxcBlob> psBlob = nullptr; // ピクセルシェーダ

	/// RootSignature
#pragma region /// RootSignature ///

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma endregion

	/// descriptorRange
#pragma region /// descriptorRange ///

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる t0
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;                       // Offsetを自動計算

#pragma endregion

	/// rootParameter
#pragma region /// rootParameter ///

	D3D12_ROOT_PARAMETER TEXTURERootParameters[4] = {};
	TEXTURERootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	TEXTURERootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	TEXTURERootParameters[0].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	TEXTURERootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	TEXTURERootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;   // VertexShaderで使う
	TEXTURERootParameters[1].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	TEXTURERootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
	TEXTURERootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderで使う
	TEXTURERootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
	TEXTURERootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	TEXTURERootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	TEXTURERootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	TEXTURERootParameters[3].Descriptor.ShaderRegister = 1;                      // レジスタ番号1を使う

	descriptionRootSignature.pParameters = TEXTURERootParameters;                // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(TEXTURERootParameters);    // 配列の長さ

#pragma endregion

	/// Sampler
#pragma region /// Sampler ///

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;         // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;       // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;                       // ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;                               // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion

	/// InputLayout
#pragma region /// InputLayout ///

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

	/// バイナリをもとに生成
#pragma region /// バイナリをもとに生成 rootSignature　///

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {

		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline->rootSignature));
	assert(SUCCEEDED(hr));

#pragma endregion

	/// BlendState
#pragma region /// BlendState ///

	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	// ブレンドモードNone D3D12_COLOR_WRITE_ENABLE_ALLだけ
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion

	/// RasterizerState
#pragma region /// RasterizerState ///

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計周り)を表示しない、背面カリング
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

	/// ShaderComplie
#pragma region /// ShaderComplie ///

	// 頂点シェーダ
	vsBlob =
		CompileShader(L"./Resources/Shaders/Object3d.VS.hlsl", L"vs_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(vsBlob != nullptr);

	// ピクセルシェーダ
	psBlob =
		CompileShader(L"./Resources/Shaders/Object3d.PS.hlsl", L"ps_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(psBlob != nullptr);

#pragma endregion

	/// DepthStencil
#pragma region /// DepthStencil ///

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depth機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行う
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

	/*========================================================================================================*/
	// Pipeline State Objectの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipeline->rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(),vsBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(),psBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	pipeline->pipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pipeline->pipelineState));
	assert(SUCCEEDED(hr));

	return pipeline;
}

//============================================================
// BlinnPhong反射適応パイプライン生成
//============================================================
std::unique_ptr<PipelineObject> Pipeline::CreateBlinnPhongGraphicsPipeline() {

	DirectXCommon* dxCommon = DirectXCommon::Instance();

	HRESULT hr;

	std::unique_ptr<PipelineObject> pipeline = std::make_unique<PipelineObject>();

	ComPtr<ID3DBlob> errorBlob = nullptr; // エラー
	ComPtr<ID3DBlob> signatureBlob = nullptr;

	ComPtr<IDxcBlob> vsBlob = nullptr; // 頂点シェーダ
	ComPtr<IDxcBlob> psBlob = nullptr; // ピクセルシェーダ

	/// RootSignature
#pragma region /// RootSignature ///

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma endregion

	/// descriptorRange
#pragma region /// descriptorRange ///

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる t0
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;                       // Offsetを自動計算

#pragma endregion

	/// rootParameter
#pragma region /// rootParameter ///

	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;   // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                      // レジスタ番号0とバインド

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;      // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;                // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;             // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;                      // レジスタ番号1を使う

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;      // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;   // PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;                      // レジスタ番号2を使う

	descriptionRootSignature.pParameters = rootParameters;                // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);    // 配列の長さ

#pragma endregion

	/// Sampler
#pragma region /// Sampler ///

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;         // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;       // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;                       // ありったけのMipMapを使う
	staticSamplers[0].ShaderRegister = 0;                               // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

#pragma endregion

	/// InputLayout
#pragma region /// InputLayout ///

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

	/// バイナリをもとに生成
#pragma region /// バイナリをもとに生成 rootSignature　///

	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {

		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline->rootSignature));
	assert(SUCCEEDED(hr));

#pragma endregion

	/// BlendState
#pragma region /// BlendState ///

	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	// ブレンドモードNone D3D12_COLOR_WRITE_ENABLE_ALLだけ
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion

	/// RasterizerState
#pragma region /// RasterizerState ///

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計周り)を表示しない、背面カリング
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

	/// ShaderComplie
#pragma region /// ShaderComplie ///

	// 頂点シェーダ
	vsBlob =
		CompileShader(L"./Resources/Shaders/BlinnPhong.VS.hlsl", L"vs_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(vsBlob != nullptr);

	// ピクセルシェーダ
	psBlob =
		CompileShader(L"./Resources/Shaders/BlinnPhong.PS.hlsl", L"ps_6_0",
			dxCommon->GetDxcUtils(), dxCommon->GetDxcCompiler(), dxCommon->GetIncludeHandler());
	assert(psBlob != nullptr);

#pragma endregion

	/// DepthStencil
#pragma region /// DepthStencil ///

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depth機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みを行う
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

	/*========================================================================================================*/
	// Pipeline State Objectの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = pipeline->rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vsBlob->GetBufferPointer(),vsBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { psBlob->GetBufferPointer(),psBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	pipeline->pipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&pipeline->pipelineState));
	assert(SUCCEEDED(hr));

	return pipeline;
}

//============================================================
// パイプライン各種生成
//============================================================
void Pipeline::CreateGraphicsPipelines() {

	pipelines_[static_cast<size_t>(PipelineType::PRIMITIVE)] = CreatePrimitiveGraphicsPipeline();
	pipelines_[static_cast<size_t>(PipelineType::TEXTURE)] = CreateTextureGraphicsPipeline();
	pipelines_[static_cast<size_t>(PipelineType::BLINNPHONG)] = CreateBlinnPhongGraphicsPipeline();
}