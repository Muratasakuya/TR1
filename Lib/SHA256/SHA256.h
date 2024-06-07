#pragma once

#include <string>

//================================================
// OpenCV Class
//================================================
class SHA256 {
public:
	//====================
	// public
	//====================

	// ハッシュ値を生成する関数
	static std::string CreateHash(const std::string& input);

private:
	//====================
	// private
	//====================


};