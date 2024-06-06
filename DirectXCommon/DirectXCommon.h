#pragma once
#include <Windows.h>

#include <dxgidebug.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>

#include <cassert>

#include "WinApp.h"
#include "Vector.h"
#include "ComPtr.h"

// 解放忘れのチェック
struct LeakChecker {

	~LeakChecker() {

		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {

			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

//================================================
// DirecXCommon Class
//================================================
class DirectXCommon {
public:
	//====================
	// public
	//====================

	// ヒープの生成を行う関数
	ComPtr<ID3D12DescriptorHeap> MakeDescriptorHeap(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors, bool shaderVisible
	);
	// Depthの生成
	ComPtr<ID3D12Resource> MakeDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	// デバッグ
	void DebugLayer();
	void DebugInfo();

	void TransitionBarrier();
	void CreateFenceEvent();

	void CommandQueue();
	void CreateSwapChain(WinApp* winApp);
	void CreateDescriptorHeap();

	void CreateDevice();
	void CreateRTV();
	void CreateDSV();
	void ClearWindow();

	void Initialize(WinApp* winApp, int32_t width, int32_t height);
	void PreDraw();
	void PostDraw();
	void Finalize(WinApp* winApp);

	// シングルトン
	static DirectXCommon* Instance();

	// getter

	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList_.Get(); }
	IDxcUtils* GetDxcUtils() const { return dxcUtils_.Get(); }
	IDxcCompiler3* GetDxcCompiler() const { return dxcCompiler_.Get(); }
	IDxcIncludeHandler* GetIncludeHandler() const { return includeHandler_.Get(); }

	DXGI_SWAP_CHAIN_DESC1& GetSwapChainDesc() { return swapChainDesc_; }
	D3D12_DESCRIPTOR_HEAP_DESC& GetRTVDesc() { return rtvDescriptorHeapDesc_; }

private:
	//====================
	// private
	//====================

	LeakChecker leakCheck;

	HRESULT hr_;

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	ComPtr<IDXGIFactory7> dxgiFactory_;
	ComPtr<IDXGIAdapter4> useAdapter_;
	ComPtr<ID3D12Device> device_;
	ComPtr<ID3D12CommandQueue> commandQueue_;
	ComPtr<ID3D12CommandAllocator> commandAllocator_;
	ComPtr<ID3D12GraphicsCommandList> commandList_;
	ComPtr<IDXGISwapChain4> swapChain_;
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };
	ComPtr<ID3D12Resource> depthStencilResource_;
	ComPtr<ID3D12Debug1> debugController_;
	ComPtr<ID3D12Fence> fence_;
	ComPtr<IDxcUtils> dxcUtils_;
	ComPtr<IDxcCompiler3> dxcCompiler_;
	ComPtr<IDxcIncludeHandler> includeHandler_;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_{};
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc_{};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	UINT backBufferIndex_;
	D3D12_RESOURCE_BARRIER barrier_{};
	uint64_t fenceValue_;
	HANDLE fenceEvent_;
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	// コピーの禁止
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;
};