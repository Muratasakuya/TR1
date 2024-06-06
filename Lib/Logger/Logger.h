#pragma once
#include <Windows.h>
#include <string>
#include <format>

//================================================
// Log出力関連
//================================================

// 関数宣言
void Log(const std::string& message);
std::wstring ConvertString(const std::string& str);
std::string ConvertWString(const std::wstring& str);