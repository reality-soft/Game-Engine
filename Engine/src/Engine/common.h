#pragma once
#include <list>
#include <vector>
#include <wrl.h>
#include <string>
#include <map>
#include <queue>
#include <windows.h>
#include <tchar.h>
#include <atlconv.h>
#include <DirectXMath.h>
#include "fmod.hpp"

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
