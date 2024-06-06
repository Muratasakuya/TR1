#include "OpenCV.h"

//============================================================
// コンストラクタ
//============================================================
OpenCV::OpenCV() {

	Initialize();
}

//============================================================
// 初期化
//============================================================
void OpenCV::Initialize() {

	// ウィンドウの名前
	//cv::namedWindow("OpenCV Window", cv::WINDOW_AUTOSIZE);

	// カメラキャプチャ
	cv::VideoCapture cap(0);

	// カメラウィンドウのサイズ
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // 横幅
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360); // 縦幅
	cap.set(cv::CAP_PROP_FPS, 60);           // フレームレート (FPS)

	// 関数で生成したカメラキャプチャの代入
	cap_ = cap;

	// 見つかっていない状態で初期化
	isFound_ = false;
}

//============================================================
// 更新処理
//============================================================
void OpenCV::Update() {

	// ウェブカメラからの画像データの読み込み
	cap_ >> frame_;

	/*-----------------------------------------------------------------------------*/



	// 取得したフレームの左右反転
	// 第3引数 : 0 = 垂直方向方向 || 1 = 水平方向
	// 今回は鏡のように反転させるため水平方向に反転させる zoomと同じ (原理は知らない)
	cv::flip(frame_, frame_, 1);

	// 平滑化処理 (ガウシアンブラー)
	// 第3引数 : カーネルサイズ (ぼかしの範囲) 必ず奇数
	// 第4引数 : σ (シグマ、標準偏差) シグマが大きいほどぼかしは強い
	cv::GaussianBlur(frame_, frame_, cv::Size(5, 5), 0);



	/*-----------------------------------------------------------------------------*/



	//// BGRからHSVへ変換
	//// BGR : Blue Green Red || HSV : Hue(色相) Saturation(彩度) Value(明度)
	//// 変換元の入力画像から変換後の出力画像に変換
	//cv::cvtColor(frame_, hsvFrame_, COLOR_BGR2HSV);

	//// 赤色の範囲をマスクする(2つの範囲を設定)
	//// 色相下限
	//cv::Scalar lowerRed1(0, 120, 70);
	//// 色相上限
	//cv::Scalar upperRed1(10, 255, 255);
	//// 設定したマスク範囲の出力
	//cv::inRange(hsvFrame_, lowerRed1, upperRed1, mask1_);

	//// 色相下限
	//cv::Scalar lowerRed2(170, 120, 70);
	//// 色相上限
	//cv::Scalar upperRed2(180, 255, 255);
	//// 設定したマスク範囲の出力
	//cv::inRange(hsvFrame_, lowerRed2, upperRed2, mask2_);

	//// 2つのマスクを結合
	//cv::bitwise_or(mask1_, mask2_, mask_);



	///*-----------------------------------------------------------------------------*/



	//// ノイズ除去のためにマスクを処理
	//cv::erode(mask_, mask_, cv::Mat(), cv::Point(-1, -1), 2);
	//cv::dilate(mask_, mask_, cv::Mat(), cv::Point(-1, -1), 2);



	///*-----------------------------------------------------------------------------*/



	//// 赤い場所の輪郭部分を検出する
	//std::vector<std::vector<cv::Point>> contours;
	//cv::findContours(mask_, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//// 最大の輪郭を見つける
	//// 最大の輪郭のインデックス、-1は見つかっていない状態
	//int largestContourIndex = -1;
	//// 最大面積
	//double largestArea = 0;
	//for (size_t i = 0; i < contours.size(); i++) {

	//	// 輪郭の面積の計算
	//	double area = cv::contourArea(contours[i]);

	//	if (area > largestArea) {

	//		// 最大面積の更新
	//		largestArea = area;

	//		// 現在の輪郭のインデックスiを保存
	//		largestContourIndex = static_cast<int>(i);
	//	}
	//}

	//// 最大の輪郭が見つかった場合、その重心を計算
	//if (largestContourIndex != -1) {

	//	// 輪郭のモーメント計算、モーメントは、形状の中心や分布を示す統計量
	//	cv::Moments moments = cv::moments(contours[largestContourIndex]);

	//	// 中心座標の計算
	//	centerPos_.x = static_cast<int>(moments.m10 / moments.m00);
	//	centerPos_.y = static_cast<int>(moments.m01 / moments.m00);

	//	isFound_ = true;
	//} else {

	//	isFound_ = false;
	//}
}


//============================================================
// 描画処理
//============================================================
void OpenCV::Draw() {

	// 対象が見つかったときに描画
	if (isFound_) {

		// 円描画
		cv::circle(frame_, cv::Point(centerPos_.x, centerPos_.y), 8, cv::Scalar(0, 255, 0), -1);
	}

	// 画像(今回はカメラのフレーム)、ウィンドウの表示
	cv::imshow("OpenCV Window", frame_);
}


//============================================================
//  終了処理
//============================================================
void OpenCV::Finalize() {

	cap_.release();
	cv::destroyAllWindows();
}