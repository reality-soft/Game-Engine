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
	imfont = ImGui::GetIO().Fonts->AddFontFromFileTTF("../../Contents/Fonts/Garet_Book.ttf", 24.0f, &fontConfig);

	// store context instance
	this->context = ImGui::GetCurrentContext();
}

void GUIMgr::RenderWidgets()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto widget : widgets)
	{
		widget.second->Update();
		widget.second->Render();
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

void GuiWidget::PreRender()
{

}

void GuiWidget::PostRender()
{
}
