#pragma once
#ifdef _DEBUG
#include "stdafx.h"
#include "DllMacro.h"
#include "DX11App.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "imfilebrowser.h"

namespace reality
{
	class DLL_API GuiWidget
	{
	public:
		GuiWidget() = default;
		~GuiWidget() = default;

	public:
		bool open_ = true;
		virtual void Init() {};
		virtual void Update() = 0;
		virtual void Render() = 0;
		void WidgetRender() {
			if (open_ == false) {
				return;
			}
			Update();
			Render();
		}
		void InvertOpen() {
			open_ = !open_;
		}
	};

	class DLL_API GUIMgr
	{
		SINGLETON(GUIMgr);
#define GUI GUIMgr::GetInst()

	public:
		void Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
		void RenderWidgets();


		template <typename WidgetType, typename... Args>
		void AddWidget(string widget_name, Args&&...args);

		template <typename WidgetType>
		WidgetType* FindWidget(string widget_name);

		ImGuiContext* GetContext();
		LPCSTR base_font_file;
		map<string, ImFont*> font_map;
		ImFont* AddFont(string font_name, LPCSTR ttf_file, float font_size);
	private:
		ImGuiContext* context;
		unordered_map<string, shared_ptr<GuiWidget>> widgets;
	};

	template <typename WidgetType, typename... Args>
	void GUIMgr::AddWidget(string widget_name, Args&&...args)
	{
		if (FindWidget<WidgetType>(widget_name) != nullptr)
			return;

		shared_ptr<GuiWidget> widget = dynamic_pointer_cast<GuiWidget>(make_shared<WidgetType>(args...));
		widget->Init();

		widgets.insert(make_pair(widget_name, widget));
	}

	template <typename WidgetType>
	WidgetType* GUIMgr::FindWidget(string widget_name)
	{
		auto iter = widgets.find(widget_name);
		if (iter == widgets.end())
			return nullptr;

		return dynamic_cast<WidgetType*>(iter->second.get());
	}
}
#endif