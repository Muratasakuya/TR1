#pragma once

/// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <string>
#include <iostream>
#include <fstream>

#include "Function.h"

/// cvの省略
using namespace cv;

///// info /////
//
//
// SHA256
// 
//   ↓
// 
// UUID生成
//
//
//

//================================================
// OpenCV Class
//================================================
class OpenCV {
public:
	//====================
	// public
	//====================

	/*========================================================*/
	/// HAJIKI_TYPE 種類
	/*========================================================*/
	enum class HAJIKI_TYPE {

		NORMAL,
		FEATHER,
		HEAVY
	};

	/*========================================================*/
	/// HAJIKI_SPEC 構造体
	/*========================================================*/
	struct HAJIKI_SPEC {

		// セキュリティ
		char securityKey[128];
		// 変換する色
		cv::Scalar hsv;
		// 読み込むデータの種類
		HAJIKI_TYPE type;
	};

	// コンストラクタ
	OpenCV();
	// デストラクタ
	~OpenCV() {};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了処理
	void Finalize();

private:
	//====================
	// private
	//====================

	// カメラキャプチャ
	cv::VideoCapture cap_;

	// OpenCVで画像を表すための基本的なデータ型
	cv::Mat
		frame_,    // カメラからキャプチャしたフレーム(画像データ)の保持
		hsvFrame_, // BGR色空間からHSV色空間に変換したフレームの保持
		mask1_,    // 赤色の第1範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask2_,    // 赤色の第2範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask_;     // mask1とmask2を論理和(OR)した結果を保持するマスク(バイナリ画像)の保持

	// 中心座標
	Vector2d centerPos_;

	// 検出できたかどうか
	bool isFound_;

	// 終了フラグ
	bool isFinish_;
};
