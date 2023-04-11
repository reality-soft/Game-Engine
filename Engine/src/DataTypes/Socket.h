#pragma once
#include "stdafx.h"

struct Socket {
public:
	Socket(int bone_id, XMMATRIX local_offset = XMMatrixIdentity()) : 
		bone_id(bone_id), 
		local_offset(local_offset) {};

public:
	int bone_id;
	XMMATRIX local_offset;
	XMMATRIX animation_matrix;
};