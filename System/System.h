//#pragma once
//#include <Windows.h>
//
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <cassert>
//
//#include "Matrix4x4.h"
//#include <dxgidebug.h>
//
//#include "ComPtr.h"
//
////================================================
//// System Class
////================================================
//class System {
//public:
//	//====================
//	// public
//	//====================
//
//	// 解放忘れのチェック
//	struct LeakChecker {
//
//		~LeakChecker() {
//
//			ComPtr<IDXGIDebug1> debug;
//			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
//
//				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
//				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
//				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
//			}
//		}
//	};
//
//	// 各システムの初期化
//	static void Initialize(int width, int height);
//
//	// フレーム開始処理
//	static void BeginFrame();
//
//	// フレーム終了処理
//	static void EndFrame();
//
//	// システムの終了
//	static void Finalize();
//
//	// メッセージの受け渡し処理
//	static bool ProcessMessage();
//
//	// 描画処理
//	static void DrawTriangle(Matrix4x4 wvpMatrix);
//	static void DrawSprite(Matrix4x4 wvpMatrix);
//	static void DrawSphere(Matrix4x4 wvpMatrix, Matrix4x4 worldMatrix, Vector3 lightDirecton);
//	static void DrawModel();
//private:
//	//====================
//	// private
//	//====================
//
//	LeakChecker leakCheck_;
//};