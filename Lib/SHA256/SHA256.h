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
	
#pragma region /// データ型 ///
	/*========================================================*/
	/// HAJIKI_TYPE 種類
	/*========================================================*/
	enum class HAJIKI_TYPE {

		NORMAL,
		FEATHER,
		HEAVY,

		// タイプの数
		TYPENUM
	};

	/*========================================================*/
	/// HAJIKI_SPEC 構造体
	/*========================================================*/
	struct HAJIKI_SPEC {

		// 読み込むデータの種類
		HAJIKI_TYPE type;
	};
#pragma endregion

	// ハッシュ値を生成する関数
	static std::string CreateHash(const std::string& input);

	// getter

	// HAJIKI_TYPEのハッシュ値を取得する関数
	std::string GetTypeHash() const;
	std::string GetTypeName() const;

	// setter

	// HAJKI_TYPEのセット
	void SetType(HAJIKI_SPEC type) { hajikiSpec_ = type; }

private:
	//====================
	// private
	//====================

	// HAJIKIの種類
	HAJIKI_SPEC hajikiSpec_;
};