#pragma once

namespace reality
{
	struct DataItem
	{
	public:
		string item_name;
	public:
		map<string, string> values;
	public:
		string	GetValue(string category) 
		{ 
			return values.find(category) != values.end() ? values[category] : ""; 
		}
		bool	SetValue(string category, string value)
		{
			if (values.find(category) == values.end())
				return false;

			

			values[category] = value;
			return true;
		}
	};

	struct DataTable
	{
	public:
		string sheet_name;
	public:
		std::vector<string> categories;
		std::map<string, shared_ptr<DataItem>> resdic_item;
	public:
		shared_ptr<DataItem>	LoadItem(string item)
		{
			if (resdic_item.find(item) != resdic_item.end())
				return resdic_item[item];
			else
				return NULL;
		}
		shared_ptr<DataItem>	AddItem(string item_name)
		{
			if (resdic_item.find(item_name) != resdic_item.end())
				return resdic_item[item_name];

			auto newItem = make_shared<DataItem>();

			newItem->item_name = item_name;

			for (auto category : categories)
				newItem->values.insert({ category, "" });

			newItem->values["Name"] = item_name;

			resdic_item.insert({ item_name, newItem });

			return resdic_item[item_name];
		}
		bool					AddCategory(string category_name, string setting_value = "")
		{
			
			if (find(categories.begin(), categories.end(), category_name) != categories.end())
				return false;

			categories.push_back(category_name);

			for (auto pair : resdic_item)
				pair.second->values.insert({ category_name, setting_value });

		}
		bool					SetItemName(string origin_name, string name_to_be_changed)
		{
			if (resdic_item.find(origin_name) == resdic_item.end() || resdic_item.find(name_to_be_changed) != resdic_item.end())
				return false;

			auto item = resdic_item[origin_name];
			resdic_item.erase(origin_name);
			item->item_name = name_to_be_changed;
			item->values["Name"] = item->item_name;
			resdic_item.insert({ item->item_name, item });
		}
	};

	

	class DLL_API DataTableMgr
	{
		SINGLETON(DataTableMgr)
#define DATA DataTableMgr::GetInst()
	private:
		string directory_;
	public:
		string directory() { return directory_; }
		void set_directory(string dir) { directory_ = dir; }
	private:
		std::map<std::string, shared_ptr<DataTable>> resdic_sheet;
	public:
		bool Init(string directory);
		void Release();
	public:
		shared_ptr<DataTable> AddNewSheet(string sheet_name);
		shared_ptr<DataTable> LoadSheet(string sheet_name);
	public:
		std::vector<string> GetAllDataSheetID();
	public:
		void LoadAllData();
		void LoadDir(string path);
		void LoadSheetFile(string fileName);
		void SaveSheetFile(string sheetName);
		void SaveSheetFileAs(string sheetName, string fileName);
		void SaveSprite(string sheetName);
		void SaveEffect(string sheetName);
		void SaveAll();
	};

}