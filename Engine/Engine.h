#pragma once
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <cassert>

#include "Pipeline.h"
#include "Function.h"
#include "Matrix4x4.h"
#include "ComPtr.h"

//================================================
// Engine Class
//================================================
class Engine {
public:
	//====================
	// public
	//====================

	// 各システムの初期化
	static void Initialize(int width, int height);

	// フレーム開始処理
	static void BeginFrame();

	// フレーム終了処理
	static void EndFrame();

	// システムの終了
	static void Finalize();

	// メッセージの受け渡し処理
	static bool ProcessMessage();

	/*-----------------------------------------------------------------------------------------*/
	/// 描画関数

	// 三角形
	static void DrawTriangle(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);

	// 三角錐
	static void DrawTriangularPrism(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);

	// モデル
	static void DrawModel(const std::string& identifier, const CBufferData* cBufferData, PipelineType pipelineType);

private:
	//====================
	// private
	//====================


};