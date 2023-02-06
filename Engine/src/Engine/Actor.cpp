#include "Actor.h"

using namespace KGCA41B;

void Actor::Regist(entt::registry& reg)
{
	ent = reg.create();
}

void Actor::InheritTransform()
{	
	if (parent_.get() != nullptr)
		comp_transform_.parent = make_shared<Transform>(parent_.get()->comp_transform_);	
}
