#pragma once
#include "common.h"
#include "DllMacro.h"
#include "ComponentSystem.h"

class DLL_API Actor
{
private:
	entt::entity ent;
	shared_ptr<Actor> parent_;

	XMMATRIX world;
	XMMATRIX local;
	// Components ...
	void UpdateT()
	{
		if (parent_.get() != nullptr)
			this->world = parent_.get()->world * this->local;
	}

};

