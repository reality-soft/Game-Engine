#pragma once
#include "UI_Image.h"
#include "UI_Button.h"

namespace reality
{
	class DLL_API UI_Slider : public UI_Image
	{
	private:
		float value_;
		float last_value_;
		bool is_changed_;
		shared_ptr<UI_Button> slider_;
	public:
		void InitSlider(string background, string normal, string hover, string push = "", string select = "", string disable = "");
		void SetSliderLocalRect(float width, float height);
		virtual void Update() override;
		void UpdateSliderPos();
		float GetValue();
		bool GetIsChanged() { return is_changed_; };
	};
}


