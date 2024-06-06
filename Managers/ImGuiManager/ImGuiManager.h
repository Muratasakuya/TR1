#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"

#include "ComPtr.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

//================================================
// ImGuiManager
//================================================
class ImGuiManager{
public:
	//====================
	// public
	//====================

	void Initialize();
	void Begin();
	void End();
	void Draw();
	void Finalize();

	static ImGuiManager* Instance();

	// getter
	ID3D12DescriptorHeap* GetSrvDescriotorHeap()const { return srvDescriptorHeap_.Get(); }
private:
	//====================
	// private
	//====================

	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	// コピーの禁止
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;
};

