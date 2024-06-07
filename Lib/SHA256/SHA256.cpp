#include "SHA256.h"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

//============================================================
// ハッシュ値を生成する関数
//============================================================
std::string SHA256::CreateHash(const std::string& input) {

	// Create a SHA256 context
	SHA256_CTX context;
	SHA256_Init(&context);

	// Add the data to be hashed
	SHA256_Update(&context, input.c_str(), input.size());

	// Finalize the hash and get the result
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_Final(hash, &context);

	// Convert the hash to a hex string
	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}

	return ss.str();
}