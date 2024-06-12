#include "SHA256.h"

#include <openssl/sha.h>
#include <openssl/evp.h>

#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <stdexcept>

//============================================================
// ハッシュ値を生成する関数
//============================================================
std::string SHA256::CreateHash(const std::string& input) {

	EVP_MD_CTX* context = EVP_MD_CTX_new();
	if (context == nullptr) {
		throw std::runtime_error("Failed to create EVP_MD_CTX");
	}

	if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
		EVP_MD_CTX_free(context);
		throw std::runtime_error("Failed to initialize digest");
	}

	if (EVP_DigestUpdate(context, input.c_str(), input.size()) != 1) {
		EVP_MD_CTX_free(context);
		throw std::runtime_error("Failed to update digest");
	}

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int lengthOfHash = 0;

	if (EVP_DigestFinal_ex(context, hash, &lengthOfHash) != 1) {
		EVP_MD_CTX_free(context);
		throw std::runtime_error("Failed to finalize digest");
	}

	EVP_MD_CTX_free(context);

	std::stringstream ss;
	for (unsigned int i = 0; i < lengthOfHash; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}

	return ss.str();
}

//============================================================
// HAJIKI_TYPEのハッシュ値を取得する関数
//============================================================
std::string SHA256::GetTypeHash() const {

	// 各タイプのハッシュ値を生成
	static const std::unordered_map<HAJIKI_TYPE, std::string> typeToString = {

		{HAJIKI_TYPE::NORMAL, "HAJIKI_TYPE::NORMAL"},
		{HAJIKI_TYPE::FEATHER, "HAJIKI_TYPE::FEATHER"},
		{HAJIKI_TYPE::HEAVY, "HAJIKI_TYPE::HEAVY"}
	};

	auto it = typeToString.find(hajikiSpec_.type);
	if (it != typeToString.end()) {

		return CreateHash(it->second);
	}

	// 未知タイプの場合のデフォルトハッシュ値を返す
	return CreateHash("UNKNOWN");
}

// HAJIKI_TYPEの名前を取得する関数
std::string SHA256::GetTypeName() const {

	static const std::unordered_map<HAJIKI_TYPE, std::string> typeToString = {

		{HAJIKI_TYPE::NORMAL, "NORMAL"},
		{HAJIKI_TYPE::FEATHER, "FEATHER"},
		{HAJIKI_TYPE::HEAVY, "HEAVY"}
	};

	auto it = typeToString.find(hajikiSpec_.type);
	if (it != typeToString.end()) {

		return it->second;
	}

	return "UNKNOWN";
}