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

void reality::PropertyWidget::Update()
{
	ImGui::SetCurrentContext(GUI->GetContext());
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void reality::PropertyWidget::Render()
{
	ImGui::Begin("Property");

	for (auto& [name, prop] : properties)
	{
		ImGui::Text(string("\n[" + name + "]\n").c_str());

		if (prop.type == typeid(float).hash_code() || prop.type == typeid(double).hash_code())
		{
			float casted_val = *static_cast<float*>(prop.value);
			ImGui::Text(to_string(casted_val).c_str());
		}

		if (prop.type == typeid(int).hash_code())
		{
			int casted_val = *static_cast<int*>(prop.value);
			ImGui::Text(to_string(casted_val).c_str());
		}

		if (prop.type == typeid(set<UINT>).hash_code())
		{
			set<UINT> casted_val = *static_cast<set<UINT>*>(prop.value);
			string items;
			for (auto& v : casted_val)
			{
				items += to_string(v) + ", ";
			}
			ImGui::Text(items.c_str());
		}

		if (prop.type == typeid(XMVECTOR).hash_code())
		{
			XMVECTOR casted_val = *static_cast<XMVECTOR*>(prop.value);
			string vector;
			vector =
				string("x : ") + to_string(casted_val.m128_f32[0]) + "\n" +
				string("y : ") + to_string(casted_val.m128_f32[1]) + "\n" +
				string("z : ") + to_string(casted_val.m128_f32[2]) + "\n" +
				string("w : ") + to_string(casted_val.m128_f32[3]) + "\n";

			ImGui::Text(vector.c_str());
		}
	}


	ImGui::End();
}
