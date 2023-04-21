#include "stdafx.h"
#include "UI_Listbox.h"
#include "Engine.h"
#include "InputMgr.h"

using namespace reality;


void UI_Listbox::InitListBox(string open_panel, 
	string closed_normal,	string opened_normal,
	string closed_hover,	string opened_hover,
	string closed_push,		string opened_push,
	string closed_select,	string opened_select,
	string closed_disable,	string opened_disable)
{
	UI_Button::Init();

	current_item_text_ = make_shared<UI_Text>();
		current_item_text_->InitText("", E_Font::ROTUNDA, { 0.0f, 10.0f });
	AddChildUI("1_CurrentItemText", current_item_text_);

	open_panel_ = make_shared<UI_Image>();
	open_panel_->InitImage(open_panel);
	InitButton(closed_normal, closed_hover, closed_push, closed_select, closed_disable);

	list_button_textures.insert({ "closed_normal", closed_normal });
	list_button_textures.insert({ "closed_hover", closed_hover });
	list_button_textures.insert({ "closed_push", closed_push });
	list_button_textures.insert({ "closed_select", closed_select });
	list_button_textures.insert({ "closed_disable", closed_disable });

	list_button_textures.insert({ "opened_normal", opened_normal });
	list_button_textures.insert({ "opened_hover", opened_hover });
	list_button_textures.insert({ "opened_push", opened_push });
	list_button_textures.insert({ "opened_select", opened_select });
	list_button_textures.insert({ "opened_disable", opened_disable });
}

void reality::UI_Listbox::InitItemButton(XMFLOAT2 min, float width, float height, float height_space, string normal, string hover, string push, string select, string disable)
{
	item_button_min_ = min;
	item_button_width_ = width;
	item_button_height_ = height; 
	item_button_height_space_ = height_space;

	item_button_textures[E_UIState::UI_NORMAL] = normal;
	item_button_textures[E_UIState::UI_HOVER] = hover;
	item_button_textures[E_UIState::UI_PUSH] = push;
	item_button_textures[E_UIState::UI_SELECT] = select;
	item_button_textures[E_UIState::UI_DISABLED] = disable;
}

void reality::UI_Listbox::SetOpenPanelLocalRectByMin(XMFLOAT2 min, float width, float height)
{
	open_panel_->SetLocalRectByMin(min, width, height);
}
void reality::UI_Listbox::SetOpenPanelLocalRectByMax(XMFLOAT2 max, float width, float height)
{
	open_panel_->SetLocalRectByMax(max, width, height);
}
void reality::UI_Listbox::SetOpenPanelLocalRectByCenter(XMFLOAT2 center, float width, float height)
{
	open_panel_->SetLocalRectByCenter(center, width, height);
}
void reality::UI_Listbox::SetOpenPanelLocalRectByMinMax(XMFLOAT2 min, XMFLOAT2 max)
{
	open_panel_->SetLocalRectByMinMax(min, max);
}

void UI_Listbox::Update()
{
	UI_Button::Update();

	if (current_state_ == E_UIState::UI_SELECT)
	{
		open_ = !open_;
		if (open_)
			OpenListBox();
		else
			CloseListBox();
	}

	if (!open_)
		return;

	// if Opened, Item Selecting continue
	for (int i = 0; i < item_button_list_.size(); i++)
	{
		if (item_button_list_[i]->GetCurrentState() == E_UIState::UI_SELECT)
		{
			item_button_list_[i]->SetCurrentState(E_UIState::UI_NORMAL);
			selected_item_num_ = i;
			SetListBoxText(item_list_[selected_item_num_]);
			open_ = !open_;
			CloseListBox();
			break;
		}
	}
}


string UI_Listbox::GetCurrentItem()
{
	return item_list_[selected_item_num_];
}
int UI_Listbox::GetCurrentIndex()
{
	return selected_item_num_;
}
string UI_Listbox::GetItem(int index)
{
	return item_list_[index];
}
string UI_Listbox::GetItem(string value)
{
	return *find(item_list_.begin(), item_list_.end(), value);
}
void UI_Listbox::SetItemSelected(int index)
{
	if (index >= item_list_.size())
		return;
	else
	{
		selected_item_num_ = index;
		SetListBoxText(item_list_[selected_item_num_]);
	}
}
void UI_Listbox::SetItemSelected(string value)
{
	auto iter = find(item_list_.begin(), item_list_.end(), value);
	if (iter == item_list_.end())
		return;
	else
	{
		for (int i = 0; i < item_list_.size(); i++)
		{
			if (value == item_list_[i])
			{
				selected_item_num_ = i;
				SetListBoxText(item_list_[selected_item_num_]);
			}
		}
	}
}
void UI_Listbox::AddItem(string value)
{
	item_list_.push_back(value);

	item_button_list_.push_back({ make_shared<UI_Button>() });
	auto new_item_button = item_button_list_[item_button_list_.size() - 1];
	new_item_button->InitButton(
		item_button_textures[E_UIState::UI_NORMAL],
		item_button_textures[E_UIState::UI_HOVER],
		item_button_textures[E_UIState::UI_PUSH],
		item_button_textures[E_UIState::UI_SELECT],
		item_button_textures[E_UIState::UI_DISABLED]
	);
	string item_ui_name = "1_" + value + " button";
	open_panel_->AddChildUI(item_ui_name, new_item_button);
	XMFLOAT2 min = { item_button_min_.x, item_button_min_.y + item_button_height_space_ * (item_button_list_.size() - 1) };
	new_item_button->SetLocalRectByMin(min, item_button_width_, item_button_height_);

	auto new_item_text = make_shared<UI_Text>();
	new_item_text->InitText(value, E_Font::ROTUNDA, {0.0f, 10.0f});
	new_item_button->AddChildUI("1_item_text", new_item_text);
}

void UI_Listbox::OpenListBox()
{
	// Button Texture Change
	InitButton(
		list_button_textures["opened_normal"], 
		list_button_textures["opened_hover"], 
		list_button_textures["opened_push"],
		list_button_textures["opened_select"], 
		list_button_textures["opened_disable"]);
	
	// Add OpenPanel to child 
	AddChildUI("1_OpenPanel", open_panel_);
}
void UI_Listbox::CloseListBox()
{
	// Button Texture Change
	InitButton(
		list_button_textures["closed_normal"], 
		list_button_textures["closed_hover"], 
		list_button_textures["closed_push"], 
		list_button_textures["closed_select"], 
		list_button_textures["closed_disable"]);

	// Delete OpenPanel to child
	DeleteChildUI("1_OpenPanel");
}

void reality::UI_Listbox::SetListBoxText(string text)
{
	current_item_text_->SetText(text);
}

