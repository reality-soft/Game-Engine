#pragma once
#include "Engine_include.h"

class SampleWidget : public KGCA41B::GuiWidget
{
public:
	int int_data = 0;
	float float_data = 0;
	string string_data;
	bool open_window = false;

	virtual void Update() override;
	virtual void Render() override;
};

