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
	//decodedText_ = qrDecoder_.detectAndDecode(frame_);
	// ランタイムライブラリエラー、治し方が分からない。
	// 上のコードがあるとランタイムライブラリエラーが出て実行不可
	// 考えられる問題, MTdなど、OpenCVと異なる、これが本命
	// メモリ不足とかネットに書いて合ったけどそれはない、ほぼ

	if (!decodedText_.empty()) {
		std::cout << "Decoded QR Code: " << decodedText_ << std::endl;
	}
}

//============================================================
// 描画処理
//============================================================
void OpenCV::Draw() {

	// 画像(今回はカメラのフレーム)、ウィンドウの表示
	cv::imshow("OpenCV Window", frame_);

	// デコードしたQRコードのテキストを表示
	if (!decodedText_.empty()) {
		cv::putText(frame_, decodedText_, cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
	}
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