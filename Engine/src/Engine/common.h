#pragma once
#include <list>
#include <vector>
#include <wrl.h>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include <windows.h>
#include <tchar.h>
#include <atlconv.h>
#include <DirectXMath.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include <functional>
#include <sstream>
#include "fmod.hpp"
#include "OpenXLSX.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;



#define SINGLETON(type)\
public:\
static type* GetInst()\
{\
	static type mgr;\
	return &mgr;\
}\
private:\
type() {}\
~type() {}


static std::wstring to_mw(const std::string& _src)
{
	USES_CONVERSION;
	return std::wstring(A2W(_src.c_str()));
}

static std::string to_wm(const std::wstring& _src)
{
	USES_CONVERSION;
	return std::string(W2A(_src.c_str()));
}

static std::vector<std::string> split(std::string input, char delimiter) {
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}
