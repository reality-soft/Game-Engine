#pragma once
#include "common.h"
#include "DllMacro.h"
#include "DX11App.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

namespace KGCA41B
{
	class DLL_API GuiWidget
	{
	public:
		GuiWidget() = default;
		~GuiWidget() = default;

	public:
		bool open_ = true;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};

	class DLL_API GUIMgr
	{
		SINGLETON(GUIMgr);
#define GUI GUIMgr::GetInst()

	public:
		void Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
		void RenderWidgets();

		void AddWidget(string widget_name, GuiWidget* widget);
		GuiWidget* FindWidget(string widget_name);

		ImGuiContext* GetContext();
		LPCSTR base_font_file;
		map<string, ImFont*> font_map;
		ImFont* AddFont(string font_name, LPCSTR ttf_file, float font_size);
	private:
		ImGuiContext* context;
		map<string, GuiWidget*> widgets;
	};
}

