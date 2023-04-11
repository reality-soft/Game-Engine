#pragma once
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Text.h"

namespace reality
{
	class DLL_API UI_Listbox : public UI_Button
	{
	private:
		unordered_map<string, string>	 list_button_textures;
	private:
		unordered_map<E_UIState, string> item_button_textures;
		XMFLOAT2	item_button_min_;
		float		item_button_width_;
		float		item_button_height_;
		float		item_button_height_space_;
		int			selected_item_num_;
	private:
		bool	open_ = false;
	private:
		vector<string>					item_list_;
		vector<shared_ptr<UI_Button>>	item_button_list_;
	private:
		shared_ptr<UI_Image>	open_panel_;
		shared_ptr<UI_Text>		current_item_text_;
	public:
		void InitListBox(string open_panel, string closed_normal, string opened_normal, 
			string closed_hover = "", string opened_hover = "",
			string closed_push = "", string opened_push = "",
			string closed_select = "", string opened_select = "",
			string closed_disable = "", string opened_disable = "");

		void InitItemButton(XMFLOAT2 min, float width, float height, float height_space, string normal, string hover, string push =" ", string select = "", string disable = "");

		void SetOpenPanelLocalRectByMin	(XMFLOAT2 min, float width, float height);
		void SetOpenPanelLocalRectByMax	(XMFLOAT2 max, float width, float height);
		void SetOpenPanelLocalRectByCenter(XMFLOAT2 center, float width, float height);
		void SetOpenPanelLocalRectByMinMax(XMFLOAT2 min, XMFLOAT2 max);
	public:
		virtual void Update() override;
	public:
		void	AddItem(string value);
		string	GetCurrentItem();
		int		GetCurrentIndex();
		string	GetItem(int index);
		string	GetItem(string value);
		void	SetItemSelected(int index);
		void	SetItemSelected(string value);
	private:
		void	OpenListBox();
		void	CloseListBox();
		void	SetListBoxText(string text);
	};
}


