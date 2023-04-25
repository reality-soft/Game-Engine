#pragma once
#include "UIBase.h"
#include "Effect.h"

namespace reality
{
	class DLL_API UI_Sprite : public UIBase
	{
	public:
		void InitSprite(string sprite_id, float speed, bool looping); 
	public:
		virtual void UpdateThisUI() override;
	private:
		TextureSprite*	sprite_data_;
		string			sprite_id_;
		float			speed_;
		bool			looping_;
	private:
		int		current_frame_;
	};
}


