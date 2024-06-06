//#include "System.h"
//
//#include <objbase.h>
//
//#include "WinApp.h"
//#include "DirectXCommon.h"
//#include "ImGuiManager.h"
//#include "TextureManager.h"
//#include "VertexObject.h"
//#include "Model.h"
//
//#include <d3d12.h>
//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx12.h>
//
//
////============================================================
//// namespace
////============================================================
//namespace {
//
//	WinApp* winApp = nullptr;
//	DirectXCommon* directXCommon = nullptr;
//	ImGuiManager* imgui = nullptr;
//	TextureManager* texture = nullptr;
//	Model* model = nullptr;
//}
//
////============================================================
//// 初期化
////============================================================
//void System::Initialize(int width, int height) {
//
//	// ComInitialize
//	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
//
//	winApp = WinApp::Instance();
//	directXCommon = DirectXCommon::Instance();
//	imgui = ImGuiManager::Instance();
//	texture = TextureManager::Instance();
//	model = Model::Instance();
//
//	// エラー、警告が出たら止める
//	directXCommon->DebugLayer();
//	// ウィンドウの生成
//	winApp->CreateMainWindow(width, height);
//
//	// DirectXの初期化
//	directXCommon->Initialize(winApp, width, height);
//
//	// ImGuiの初期化
//	imgui->Initialize();
//
//	// Texture
//	texture->Initialize();
//}
//
////============================================================
//// フレーム開始処理
////============================================================
//void System::BeginFrame() {
//
//	imgui->Begin();
//	directXCommon->PreDraw();
//}
//
////============================================================
//// フレーム終了処理
////============================================================
//void System::EndFrame() {
//
//	imgui->End();
//	imgui->Draw();
//	directXCommon->PostDraw();
//}
//
////============================================================
//// 終了処理
////============================================================
//void System::Finalize() {
//
//	directXCommon->Finalize(winApp);
//	imgui->Finalize();
//
//	// ComFinalize
//	CoUninitialize();
//}
//
////============================================================
//// メッセージの受け渡し処理
////============================================================
//bool System::ProcessMessage() {
//
//	if (winApp->ProcessMessage()) {
//		return 1;
//	}
//	else {
//		return 0;
//	}
//}
//
////============================================================
//// 描画処理
////============================================================
//// 三角形
//void System::DrawTriangle(Matrix4x4 wvpMatrix) {
//
//	// CommandListをdxCommonClassからもってくる
//	ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon->GetCommandList();
//
//	VertexObject& vertex = VertexObject::Instance();
//
//	// SRVの更新
//	texture->Update();
//	// Vertexの設定
//	vertex.CreateTriangleVertexResource(wvpMatrix);
//
//	// RootSignatureを設定、PSOに設定してても別途設定が必要
//	commandList->SetGraphicsRootSignature(directXCommon->GetRootSignature_().Get());
//	commandList->SetPipelineState(directXCommon->GetGraphicsPipelineState_().Get());
//	commandList->IASetVertexBuffers(0, 1, &vertex.GetVertexBufferView());
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// マテリアルCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(0, vertex.GetMaterialResource()->GetGPUVirtualAddress());
//	// wvp用のCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(1, vertex.GetWvpResource()->GetGPUVirtualAddress());
//	// SRVのDescriptorTableの先頭を設定。2はrootParamater[2]
//	commandList->SetGraphicsRootDescriptorTable(2, texture->GetTextureSrvHandleGPU());
//	// 描画を行う(DrawCall)。3頂点で1つのインスタンス
//	commandList->DrawInstanced(6, 1, 0, 0);
//}
//
//// Sprite画像
//void System::DrawSprite(Matrix4x4 wvpMatrix) {
//
//	// CommandListをdxCommonClassからもってくる
//	ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon->GetCommandList();
//
//	VertexObject& vertex = VertexObject::Instance();
//
//	// SRVの更新
//	texture->Update();
//	// Vertexの設定
//	vertex.CreateSpriteVertexResource(wvpMatrix);
//
//	// RootSignatureを設定、PSOに設定してても別途設定が必要
//	commandList->SetGraphicsRootSignature(directXCommon->GetRootSignature_().Get());
//	commandList->SetPipelineState(directXCommon->GetGraphicsPipelineState_().Get());
//	commandList->IASetVertexBuffers(0, 1, &vertex.GetVertexBufferViewSprite());
//	// IBVを設定
//	commandList->IASetIndexBuffer(&vertex.GetIndexBufferViewSprite());
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// マテリアルCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(0, vertex.GetMaterialResourceSprite()->GetGPUVirtualAddress());
//	// wvp用のCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(1, vertex.GetTransformationMatrixResourceSprite()->GetGPUVirtualAddress());
//	// SRVのDescriptorTableの先頭を設定。2はrootParamater[2]
//	commandList->SetGraphicsRootDescriptorTable(2, texture->GetTextureSrvHandleGPU());
//	// 描画を行う(DrawCall)。6個のインデックスを使用し1つのインスタンスを描画
//	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
//}
//
//// Sphere
//void System::DrawSphere(Matrix4x4 wvpMatrix, Matrix4x4 worldMatrix, Vector3 lightDirecton) {
//
//	// CommandListをdxCommonClassからもってくる
//	ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon->GetCommandList();
//
//	VertexObject& vertex = VertexObject::Instance();
//
//	// SRVの更新
//	texture->Update();
//	// Vertexの設定
//	vertex.CreateSphereVertexResource(wvpMatrix, worldMatrix);
//	vertex.CreateLightResource(lightDirecton);
//
//	// RootSignatureを設定、PSOに設定してても別途設定が必要
//	commandList->SetGraphicsRootSignature(directXCommon->GetRootSignature_().Get());
//	commandList->SetPipelineState(directXCommon->GetGraphicsPipelineState_().Get());
//	commandList->IASetVertexBuffers(0, 1, &vertex.GetVertexBufferViewSphere());
//	commandList->IASetVertexBuffers(1, 1, &vertex.GetLightBufferView());
//
//	// IBVを設定
//	commandList->IASetIndexBuffer(&vertex.GetIndexBufferViewSphere());
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// マテリアルCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(0, vertex.GetMaterialResourceSphere()->GetGPUVirtualAddress());
//	// wvp用のCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(1, vertex.GetWvpResourceSphere()->GetGPUVirtualAddress());
//	// light用のBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(3, vertex.GetLightResource()->GetGPUVirtualAddress());
//	// SRVのDescriptorTableの先頭を設定。2はrootParamater[2]
//	commandList->SetGraphicsRootDescriptorTable(2, texture->GetTextureSrvHandleGPU2());
//	// 描画を行う(DrawCall)。3頂点で1つのインスタンス
//	commandList->DrawIndexedInstanced(1536, 1, 0, 0, 0);
//}
//
//// Model
//void System::DrawModel() {
//
//	// CommandListをdxCommonClassからもってくる
//	ComPtr<ID3D12GraphicsCommandList> commandList = directXCommon->GetCommandList();
//
//	model->Create();
//
//	// SRVの更新
//	texture->Update();
//
//	// RootSignatureを設定、PSOに設定してても別途設定が必要
//	commandList->SetGraphicsRootSignature(directXCommon->GetRootSignature_().Get());
//	commandList->SetPipelineState(directXCommon->GetGraphicsPipelineState_().Get());
//	commandList->IASetVertexBuffers(0, 1, &model->GetVertexBufferView());
//	commandList->IASetVertexBuffers(1, 1, &model->GetLightBufferView());
//
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考える
//	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// マテリアルCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(0, model->GetMaterialResource()->GetGPUVirtualAddress());
//	// wvp用のCBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(1, model->GetWvpResource()->GetGPUVirtualAddress());
//	// light用のBufferの場所を設定
//	commandList->SetGraphicsRootConstantBufferView(3, model->GetLightResource()->GetGPUVirtualAddress());
//	// SRVのDescriptorTableの先頭を設定。2はrootParamater[2]
//	commandList->SetGraphicsRootDescriptorTable(2, texture->GetTextureSrvHandleGPU2());
//	// 描画を行う(DrawCall)。3頂点で1つのインスタンス
//	commandList->DrawInstanced(UINT(model->GetModelData().vertices.size()), 1, 0, 0);
//}