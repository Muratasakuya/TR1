#include "ImGuiManager.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

//============================================================
// シングルトンインスタンス
//============================================================
ImGuiManager* ImGuiManager::Instance() {
	static ImGuiManager instance;
	return &instance;
}

//============================================================
// 初期化
//============================================================
void ImGuiManager::Initialize() {

	DirectXCommon* directXCommon = DirectXCommon::Instance();
	WinApp* winApp = WinApp::Instance();

	// デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.NumDescriptors = 128;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// デスクリプタヒープ生成
	HRESULT hr = directXCommon->GetDevice()->CreateDescriptorHeap(
		&srvDesc, IID_PPV_ARGS(&srvDescriptorHeap_));
	assert(SUCCEEDED(hr));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		directXCommon->GetDevice(),
		directXCommon->GetSwapChainDesc().BufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);
}

//============================================================
// フレームの開始
//============================================================
void ImGuiManager::Begin() {
#ifdef _DEBUG

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

//============================================================
// 内部コマンドの生成
//============================================================
void ImGuiManager::End() {
#ifdef _DEBUG

	// デモ用のUi表示
	//ImGui::ShowDemoWindow();

	ImGui::Render();
#endif
}

//============================================================
// 終了処理
//============================================================
void ImGuiManager::Finalize() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	srvDescriptorHeap_.Reset();
#endif
}

//============================================================
// 描画
//============================================================
void ImGuiManager::Draw() {
#ifdef _DEBUG

	DirectXCommon* directXCommon = DirectXCommon::Instance();

	ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon->GetCommandList();
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_.Get()};
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
#endif
}