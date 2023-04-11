#pragma once
#include "stdafx.h"

struct Socket {
public:
	Socket(string bone_name, XMMATRIX local_offset = XMMatrixIdentity()) : 
		bone_name(bone_name), 
		local_offset(local_offset) {};

public:
	string bone_name;
	XMMATRIX local_offset;
	XMMATRIX animation_matrix;
};