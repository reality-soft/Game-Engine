#include "stdafx.h"
#include "GUIMgr.h"

using namespace KGCA41B;

void GUIMgr::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);


	// Create an ImFontConfig object
	ImFontConfig fontConfig;
	fontConfig.SizePixels = 1.f;

	// Add a font to the font manager
	base_font_file = "../../Contents/Fonts/Garet_Book.ttf";
	AddFont("base_font", base_font_file, 18.0f);

	// store context instance
	this->context = ImGui::GetCurrentContext();
}

void GUIMgr::RenderWidgets()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto& widget : widgets)
	{		
		widget.second->WidgetRender();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	ImGui::EndFrame();
}

void GUIMgr::AddWidget(string widget_name, GuiWidget* widget)
{
	if (FindWidget(widget_name) != nullptr)
		return;

	widget->Init();
	widgets.insert(make_pair(widget_name, widget));
}

GuiWidget* GUIMgr::FindWidget(string widget_name)
{
	auto iter = widgets.find(widget_name);
	if (iter != widgets.end())
		return iter->second;
	
	return nullptr;
}

ImGuiContext* KGCA41B::GUIMgr::GetContext()
{
	return context;
}

ImFont* KGCA41B::GUIMgr::AddFont(string font_name, LPCSTR ttf_file, float font_size)
{
	ImFont* new_font;

	if (ttf_file == nullptr)
	{
		new_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(base_font_file, font_size, nullptr);
	}
	else
	{
		new_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(ttf_file, font_size, nullptr);
	}

	font_map.insert(make_pair(font_name, new_font));

	return new_font;
}
