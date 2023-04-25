#include "stdafx.h"
#include "UI_Sprite.h"
#include "ResourceMgr.h"
#include "TimeMgr.h"

using namespace reality;

void UI_Sprite::InitSprite(string sprite_id, float speed, bool looping)
{
	UIBase::Init();
	sprite_id_ = sprite_id;
	sprite_data_ = (TextureSprite*)RESOURCE->UseResource<Sprite>(sprite_id_);
	speed_ = speed;
	looping_ = looping;
	current_frame_ = 0;
}

void reality::UI_Sprite::UpdateThisUI()
{
	if (sprite_data_ == nullptr)
		return;

	if (current_frame_ < sprite_data_->tex_id_list.size())
		current_frame_ += speed_ * TIMER->GetDeltaTime();
	else if (looping_)
		current_frame_ = 0;
	else
		current_frame_ = sprite_data_->tex_id_list.size() - 1;

	render_data_.tex_id = sprite_data_->tex_id_list[current_frame_];
}
