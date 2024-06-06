#include "DirectXCommon.h"

// lib /* .hに書いてはいけない */
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//============================================================
// シングルトンインスタンス
//============================================================
DirectXCommon* DirectXCommon::Instance() {
	static DirectXCommon instance;
	return &instance;
}

//============================================================
// デバッグの表示、エラー警告
//============================================================
void DirectXCommon::DebugLayer() {

#ifdef _DEBUG

	debugController_ = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {

		// デバッグレイヤーを有効化する
		debugController_->EnableDebugLayer();

		// さらにGPU側でもチェックを行うようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif
}

//============================================================
// エラー、警告が出たら停止させる
//============================================================
void DirectXCommon::DebugInfo() {

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {

			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
#endif
}

//============================================================
// DepthStencilTextureを作成する関数
//============================================================
ComPtr<ID3D12Resource> DirectXCommon::MakeDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                                   // Textureの幅
	resourceDesc.Height = height;                                 // Textureの高さ
	resourceDesc.MipLevels = 1;                                   // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                            // 奥行　or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;                            // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット、Resourceに合わせる

	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,                  // Heapの設定
		D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定、特になし
		&resourceDesc,                    // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue,                 // Clear最適値
		IID_PPV_ARGS(&resource)           // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

//============================================================
// DescriptorHeapを作成する関数
//============================================================
ComPtr<ID3D12DescriptorHeap> DirectXCommon::MakeDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	UINT numDescriptors, bool shaderVisible) {

	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags =
		shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	return descriptorHeap;
}

//============================================================
// TransitionBarrier
//============================================================
void DirectXCommon::TransitionBarrier() {

	// 今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを貼る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex_].Get();
	// 遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);
}

//============================================================
// FenceとEventの生成
//============================================================
void DirectXCommon::CreateFenceEvent() {

	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0;
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	// FenceのSignalを待つためのイベントの作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

//============================================================
// CommandListをGPUに投げる、コマンドキューの生成
//============================================================
void DirectXCommon::CommandQueue() {

	commandQueue_ = nullptr;
	hr_ = device_->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue_));

	// コマンドキューの生成がうまくいったかどうか
	assert(SUCCEEDED(hr_));

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

	// コマンドアロケータの生成がうまくいったかどうか
	assert(SUCCEEDED(hr_));

	// コマンドリストを生成する
	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));

	// コマンドリストの生成がうまくいったかどうか
	assert(SUCCEEDED(hr_));
}

//============================================================
// SwapChainの生成
//============================================================
void DirectXCommon::CreateSwapChain(WinApp* winApp) {

	swapChain_ = nullptr;
	swapChainDesc_.Width = kClientWidth_;                           // 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = kClientHeight_;                         // 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;             // 色の形式
	swapChainDesc_.SampleDesc.Count = 1;                            // マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;                                 // ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // モニタにうつしたら、中身を破壊

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), winApp->GetHwnd(), &swapChainDesc_, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));

	assert(SUCCEEDED(hr_));
}

//============================================================
// DescriptorHeapの生成
//============================================================
void DirectXCommon::CreateDescriptorHeap() {

	rtvDescriptorHeap_ = nullptr;

	rtvDescriptorHeap_ =
		MakeDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	rtvDescriptorHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;   // レンダーターゲットビュー用
	rtvDescriptorHeapDesc_.NumDescriptors = 2;                      // ダブルバッファ用に2つ、多くても別に構わない
	hr_ = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc_, IID_PPV_ARGS(&rtvDescriptorHeap_));

	// ディスクリプタ―ヒープが作れたかどうか
	assert(SUCCEEDED(hr_));

	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));

	// うまく取得できたかどうか
	assert(SUCCEEDED(hr_));

	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));

	// うまく取得できたかどうか
	assert(SUCCEEDED(hr_));
}

//============================================================
// Render Terger VIewの生成
//============================================================
void DirectXCommon::CreateRTV() {

	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;        // 出力結果をSRGBも変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;   // 2dテクスチャとして書き込む

	// ディスクリプタの先頭を取得する
	rtvStartHandle_ = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	// 1つ目は最初のところに作る、作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle_;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);

	// 2つ目のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

//============================================================
// Depth Stencil VIewの生成
//============================================================
void DirectXCommon::CreateDSV() {

	// DepthStencilResourceの生成
	depthStencilResource_ = MakeDepthStencilTextureResource(device_.Get(), kClientWidth_, kClientHeight_);

	// DSV用のヒープでディスクリプタの数は1、DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = MakeDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;        // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	// DSVHeapの先頭に作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

//============================================================
// 画面のクリア
//============================================================
void DirectXCommon::ClearWindow() {

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex_], false, &dsvHandle);
	// 指定した色で画面全体をクリアする
	float crearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	// 青っぽい色、RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex_], crearColor, 0, nullptr);

	// 指定した深度で画面全体をクリアする、深度バッファクリア
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

//============================================================
// Deviceの生成
//============================================================
void DirectXCommon::CreateDevice() {

	// DXGIファクトリーの生成
	dxgiFactory_ = nullptr;

	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr_));

	// 使用するアダプタ(GPU)を決定する

	// 使用するアダプタ用の変数、最初にnullptrを入れておく
	useAdapter_ = nullptr;

	// 良い順にアダプタを頼む
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(
			i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {

		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_)); // 取得できないなんてことはない → 一大事

		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {

			// forループを抜ける
			break;
		}

		// ソフトウェアアダプタの場合は見なかったことにする
		useAdapter_ = nullptr;
	}

	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);

	// D3D12Deviceの生成

	// 最初にnullptrを入れておく
	device_ = nullptr;

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featuerLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featuerLevelStrings[] = { "12.2","12.1","12.0" };

	// 高い順に生成出来るか試していく
	for (size_t i = 0; i < _countof(featuerLevels); i++) {

		// 採用したアダプタでデバイスを生成
		hr_ = D3D12CreateDevice(useAdapter_.Get(), featuerLevels[i], IID_PPV_ARGS(&device_));

		// 指定した機能レベルでデバイスを生成できたかを確認
		if (SUCCEEDED(hr_)) {

			// ループを抜ける
			break;
		}
	}

	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
}

//============================================================
// DirectXの初期化
//============================================================
void DirectXCommon::Initialize(WinApp* winApp, int32_t width, int32_t height) {

	// デバッグの表示、エラー警告
	DebugLayer();

	// 変数の値の代入
	kClientWidth_ = width;
	kClientHeight_ = height;

	// デバイスの生成
	CreateDevice();

	// エラー、警告があれば起動できない
	DebugInfo();

	// FenceとEventの生成
	CreateFenceEvent();

	// dxCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));

	// 現時点でincludeしないが、includeするための設定を行っておく
	includeHandler_ = nullptr;
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));

	// 画面をクリアするための関数の処理の呼び出し
	CommandQueue();
	CreateSwapChain(winApp);
	CreateDescriptorHeap();

	// RTVの生成
	CreateRTV();

	// 深度バッファ
	CreateDSV();
}

//============================================================
// 描画前の準備処理
//============================================================
void DirectXCommon::PreDraw() {

	// これから書き込むバックバッファのインデックスを取得
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

	TransitionBarrier();

	// 画面のクリア
	ClearWindow();

	// ビューポートの設定
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<FLOAT>(kClientWidth_);
	viewport_.Height = static_cast<FLOAT>(kClientHeight_);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	commandList_->RSSetViewports(1, &viewport_);

	// シザー矩形の設定
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(kClientWidth_);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(kClientHeight_);

	commandList_->RSSetScissorRects(1, &scissorRect_);
}

//============================================================
// 描画後の後片付け処理
//============================================================
void DirectXCommon::PostDraw() {

	// 画面に描く処理は全て終わり、画面に映すので、状態を遷移
	// 今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseする
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	// Feneceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	if (fence_->GetCompletedValue() < fenceValue_) {

		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);

		// このあとでGPUの実行を待ってからResetが呼び出される
	}

	// 次のフレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

//============================================================
// 解放処理
//============================================================
void DirectXCommon::Finalize(WinApp* winApp) {

	CloseHandle(fenceEvent_);

	CloseWindow(winApp->GetHwnd());
}