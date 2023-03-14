#include "TestWidget.h"

void reality::TestWidget::Update()
{
	ImGui::SetCurrentContext(GUI->GetContext());
}

void reality::TestWidget::Render()
{
	
	ImGui::Begin("Render Option Window", &open_);

	string fps = "Mouse Picking : (" + to_string(picking_.m128_f32[0]) + ", " + to_string(picking_.m128_f32[1]) + ", " + to_string(picking_.m128_f32[2]) + ") ";
	ImGui::Text(fps.c_str());

	ImGui::End();
}

void reality::TestWidget::SetPickingVector(XMVECTOR picking)
{
	picking_ = picking;
}
