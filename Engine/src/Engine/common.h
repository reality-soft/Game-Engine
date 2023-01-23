#pragma once
#include <list>
#include <vector>
#include <wrl.h>
#include <string>
#include <map>
#include <windows.h>
#include <tchar.h>
#include <atlconv.h>
#include <DirectXMath.h>

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