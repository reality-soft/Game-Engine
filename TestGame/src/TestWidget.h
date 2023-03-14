#pragma once

#include "Engine_include.h"

namespace reality
{
	class TestWidget : public reality::GuiWidget
	{
	public:
		XMVECTOR picking_;
	public:
		virtual void Update() override;
		virtual void Render() override;
	public:
		void SetPickingVector(XMVECTOR picking);
	};
}


