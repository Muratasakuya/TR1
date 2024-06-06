#pragma once
#include <Windows.h>
#include <cstdint>

#include "Logger.h"

//================================================
// Windows Application Class
//================================================
class WinApp {
public:
	//====================
	// public
	//====================

	bool ProcessMessage();
	void CreateMainWindow(int32_t width, int32_t height);

	static WinApp* Instance();

	// getter
	HWND GetHwnd()const { return hwnd_; }

private:
	//====================
	// private
	//====================

	// windowHandle
	HWND hwnd_;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void RegisterWindowClass();

	WinApp() = default;
	~WinApp() = default;

	// コピー禁止
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};