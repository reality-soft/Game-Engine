#pragma once
#include "stdafx.h"

struct Socket {
public:
	Socket(UINT bone_id, XMMATRIX owner_local, XMMATRIX local_offset = XMMatrixIdentity()) :
		bone_id(bone_id),
		owner_local(owner_local),
		local_offset(local_offset) {};
	Socket() {};

public:
	int bone_id;
	XMMATRIX owner_local;
	XMMATRIX local_offset;
	XMMATRIX animation_matrix;
};