#include "GameScene.h"

#include "ImGuiManager.h"

//============================================================
// コンストラクタ
//============================================================
GameScene::GameScene() {

	/*======================================================*/
	// OpenCV

	// 生成
	openCV_ = std::make_unique<OpenCV>();

	/*======================================================*/
	// OpenSSL SHA256 ハッシュ

	// 生成
	sha_.resize(static_cast<size_t>(SHA256::HAJIKI_TYPE::TYPENUM));

	for (int i = 0; i < sha_.size(); i++) {

		sha_[i] = std::make_unique<SHA256>();
	}

	// 各HAJIKI_TYPEを設定
	sha_[0]->SetType({ SHA256::HAJIKI_TYPE::NORMAL });
	sha_[1]->SetType({ SHA256::HAJIKI_TYPE::FEATHER });
	sha_[2]->SetType({ SHA256::HAJIKI_TYPE::HEAVY });
}

//============================================================
// デストラクタ
//============================================================
GameScene::~GameScene() {

	// 解放
	openCV_->Finalize();
	openCV_.reset();
	sha_.clear();
}

//============================================================
// 初期化
//============================================================
void GameScene::Initialize() {

	/*======================================================*/
	// OepnCV

	openCV_->Initialize();
}

//============================================================
// 更新処理
//============================================================
void GameScene::Update() {

	/*======================================================*/
	// IｍGui

	ImGui::Begin("Hash Type");

	for (const auto& sha : sha_) {

		// それぞれのタイプのハッシュ値の出力
		ImGui::Text("Type: %s, Hash: %s", sha->GetTypeName().c_str(), sha->GetTypeHash().c_str());
	}

	ImGui::End();

	ImGui::Begin("QRCode");

	// 新しいQRコードデータを取得
	std::string qrCodeData = openCV_->GetQRCodeData();
	if (!qrCodeData.empty()) {
		// すでに同じタイプのものがあるかどうかをチェック
		if (std::find(keepQRCodeData_.begin(), keepQRCodeData_.end(), qrCodeData) == keepQRCodeData_.end()) {

			keepQRCodeData_.push_back(qrCodeData);
		}
	}

	// QRコードの値を表示
	for (const auto& data : keepQRCodeData_) {

		bool found = false;
		for (const auto& sha : sha_) {
			if (data == sha->GetTypeHash()) {

				// QRコードの値がshaのハッシュ値と一致する場合、TypeNameを表示
				ImGui::Text("QRCodeHash: %s (same Type: %s)", data.c_str(), sha->GetTypeName().c_str());
				found = true;
				break;
			}
		}
		if (!found) {

			// 一致するものがなかったらTypeNONEでQRコードの値を表示
			ImGui::Text("QRCodeHash: %s (same Type: NONE)", data.c_str());
		}
	}

	ImGui::End();

	/*======================================================*/
	// OpenCV

	openCV_->Update();

}

//============================================================
// 描画処理
//============================================================
void GameScene::Draw() {

	/*======================================================*/
	// OepnCV

	openCV_->Draw();

}