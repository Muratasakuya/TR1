#include "OpenCV.h"

//============================================================
// コンストラクタ
//============================================================
OpenCV::OpenCV() : cap_(0) {

	Initialize();
}

//============================================================
// デストラクタ
//============================================================
OpenCV::~OpenCV() {
	Finalize();
}

//============================================================
// 初期化
//============================================================
void OpenCV::Initialize() {

	// ウィンドウの名前
	cv::namedWindow("OpenCV Window", cv::WINDOW_AUTOSIZE);

	// カメラが開けなかったら
	if (!cap_.isOpened()) {

		// エラーメッセージを表示する
		std::cerr << "Failed to open camera!" << std::endl;
	}

	// カメラウィンドウのサイズ
	cap_.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // 横幅
	cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 360); // 縦幅
	cap_.set(cv::CAP_PROP_FPS, 60);           // フレームレート (FPS)
}

//============================================================
// 更新処理
//============================================================
void OpenCV::Update() {

	// ウェブカメラからの画像データの読み込み
	cap_ >> frame_;

	if (frame_.empty()) {
		return;
	}

	// 取得したフレームの左右反転
	cv::flip(frame_, frame_, 1);

	// 平滑化処理 (ガウシアンブラー)
	cv::GaussianBlur(frame_, frame_, cv::Size(5, 5), 0);

	// QRコードの検出とデコード
	decodedText_ = qrDecoder_.detectAndDecode(frame_);

	if (!decodedText_.empty()) {

		// 新しいQRコードデータを追加
		qrCodeData_.push_back(decodedText_);
	}
}

//============================================================
// 描画処理
//============================================================
void OpenCV::Draw() {

	// 画像(今回はカメラのフレーム)、ウィンドウの表示
	cv::imshow("OpenCV Window", frame_);
}

//============================================================
// 終了処理
//============================================================
void OpenCV::Finalize() {

	if (cap_.isOpened()) {
		cap_.release();
	}
	cv::destroyAllWindows();
}

//============================================================
// 複数のQRコードデータを取得
//============================================================
std::string OpenCV::GetQRCodeData() {

	if (!qrCodeData_.empty()) {

		// 最新のデータを取得
		std::string data = qrCodeData_.back(); 

		// 取得したデータを削除
		qrCodeData_.pop_back();

		return data;
	}
	return "";
}