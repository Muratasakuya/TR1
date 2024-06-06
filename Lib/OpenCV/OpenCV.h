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

//================================================
// OpenCV Class
//================================================
class OpenCV{
public:
	//====================
	// public
	//====================

	// コンストラクタ
	OpenCV() {};
	// デストラクタ
	~OpenCV() {};

	// 更新処理
	void Update();

private:
	//====================
	// private
	//====================


};
