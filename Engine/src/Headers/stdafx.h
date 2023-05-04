#pragma once
#pragma warning(disable : 4275) 
#pragma warning(disable : 4819) 
#pragma warning(disable : 4267) 
#pragma warning(disable : 4101) 
#pragma warning(disable : 4251) 
#pragma warning(disable : 4244) 
#pragma warning(disable : 4099)
#pragma warning(disable : 26495)
#pragma warning(disable : 26451)

#ifdef _DEBUG
#include <fbxsdk.h>
#endif
#include <fmod.hpp>
#include <entt.hpp>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <wrl.h>
#include <thread>
#include <mutex>
#include <future>
#include <cmath>
#include <random>
#define _USE_MATH_DEFINES

#include <windows.h>
#include <tchar.h>
#include <typeinfo>
#include <atlconv.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <functional>
#include <sstream>

#include "GlobalFunctions.h"
#include "DllMacro.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

#define SINGLETON(type)\
public:\
static type* GetInst()\
{\
	static type mgr;\
	return &mgr;\
}\
private:\
type() {}\
~type() {}\
type(const type&) = delete;\
type& operator=(const type&) = delete;

#define RGB_TO_FLOAT(r, g, b) XMFLOAT4(r / 256.f, g / 256.f, b / 256.f, 1.f)
#define RGBA_TO_FLOAT(r, g, b, a) XMFLOAT4(r / 256.f, g / 256.f, b / 256.f, a / 256.f)

#define _XMFLOAT4(xmvector) (DirectX::XMFLOAT4(xmvector.m128_f32))
#define _XMFLOAT3(xmvector) (DirectX::XMFLOAT3(xmvector.m128_f32))
#define _XMFLOAT2(xmvector) (DirectX::XMFLOAT2(xmvector.m128_f32))
							
#define _XMVECTOR4(xmfloat) (DirectX::XMLoadFloat4(&xmfloat))
#define _XMVECTOR3(xmfloat) (DirectX::XMLoadFloat3(&xmfloat))
#define _XMVECTOR2(xmfloat) (DirectX::XMLoadFloat2(&xmfloat))

#define TO_RAD(degree) (DirectX::XMConvertToRadians(degree))
