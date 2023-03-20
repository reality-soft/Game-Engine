#include "stdafx.h"
#include "DataTableMgr.h"
#include <fstream>
#include <io.h>
#include "ResourceMgr.h"

using namespace reality;

using std::fstream;
using std::stringstream;
using std::ios;

bool DataTableMgr::Init(string directory)
{
	set_directory(directory);
	LoadAllData();
	return true;
}

void DataTableMgr::Release()
{
	SaveAll();
}

shared_ptr<DataTable> reality::DataTableMgr::AddNewSheet(string sheet_name)
{
	if (resdic_sheet.find(sheet_name) != resdic_sheet.end())
		return resdic_sheet[sheet_name];

	shared_ptr<DataTable> newSheet = std::make_shared<DataTable>();
	newSheet->sheet_name = sheet_name;

	newSheet->AddCategory("Name");

	resdic_sheet.insert({ sheet_name, newSheet });

	return newSheet;
}
shared_ptr<DataTable> reality::DataTableMgr::LoadSheet(string sheet_name)
{
	if (resdic_sheet.find(sheet_name) != resdic_sheet.end())
		return resdic_sheet[sheet_name];
	else
		return NULL;
}


std::vector<string> reality::DataTableMgr::GetAllDataSheetID()
{
	vector<string> id_set;
	for (auto pair : resdic_sheet)
		id_set.push_back(pair.first);
	return id_set;
}

void DataTableMgr::LoadAllData()
{
	LoadDir(directory_);
}

void DataTableMgr::LoadDir(string path)
{
	string tempAdd = path + "/" + "*.*";
	intptr_t handle;
	struct _finddata_t fd;
	handle = _findfirst(tempAdd.c_str(), &fd);

	if (handle == -1L) return;

	do {
		if ((fd.attrib & _A_SUBDIR) && (fd.name[0] != '.'))
		{
			LoadDir(path + fd.name + "/");
		}
		else if (fd.name[0] != '.')
		{
			LoadSheetFile(path + "/" + fd.name);
		}
	} while (_findnext(handle, &fd) == 0);
}

void DataTableMgr::LoadSheetFile(string path)
{
	fstream fs;
	fs.open(path, ios::in);
	if (fs.fail())
		return;

	shared_ptr<DataTable> newSheet = std::make_shared<DataTable>();
	auto splited_str = split(path, '/');
	auto strs2 = split(splited_str[max((int)splited_str.size() - 1, 0)], '.');

	newSheet->sheet_name = strs2[0];

	string line;
	string value;

	// 카테고리 먼저 입력받기
	getline(fs, line);
	stringstream firstLine(line);
	while (std::getline(firstLine, value, ','))
	{
		newSheet->categories.push_back(value);
	}

	// 입력받은 카테고리 별로 데이터 입력받기
	while (!fs.eof())
	{
		if (!getline(fs, line))
			break;
		auto data = std::make_shared<DataItem>();
		stringstream lineStream(line);
		int i = 0;
		while (getline(lineStream, value, ','))
		{
			data->values.insert({ newSheet->categories[i++], value });
		}
		newSheet->resdic_item.insert({ data->values["Name"], data });
	}
	resdic_sheet.insert({ newSheet->sheet_name , newSheet });
	fs.close();
}
void DataTableMgr::SaveSheetFile(string sheetName)
{
	if (resdic_sheet.find(sheetName) == resdic_sheet.end())
		return;

	fstream fs;
	fs.open(directory_ + '/' + sheetName + ".csv", ios::out);
	if (fs.fail())
		return;

	string line = "\n";

	auto sheet = resdic_sheet[sheetName];

	// 카테고리 먼저 출력하기
	for (auto& category : sheet->categories)
	{
		if (fs.is_open())
		{
			string str = category + ',';
			fs.write(str.c_str(), str.size());
		}

	}

	if (fs.is_open())
	{
		fs.write(line.c_str(), line.size());
	}

	for (auto& pair : sheet->resdic_item)
	{
		auto data = pair.second;

		for (auto& category : sheet->categories)
		{
			if (fs.is_open())
			{
				string str2 = data->values[category] + ',';
				fs.write(str2.c_str(), str2.size());
			}

		}
		if (fs.is_open())
		{
			fs.write(line.c_str(), line.size());
		}
	}

	fs.close();
}
void DataTableMgr::SaveSheetFileAs(string sheetName, string fileName)
{
	if (resdic_sheet.find(sheetName) == resdic_sheet.end())
		return;

	fileName += ".csv";

	fstream fs;
	fs.open(directory_ + '/' + fileName, ios::out);
	if (fs.fail())
		return;

	string line = "\n";

	auto sheet = resdic_sheet[sheetName];

	// 카테고리 먼저 출력하기
	for (auto& category : sheet->categories)
	{
		if (fs.is_open())
		{
			string str = category + ',';
			fs.write(str.c_str(), str.size());
		}
	}

	if (fs.is_open())
	{
		fs.write(line.c_str(), line.size());
	}

	for (auto& pair : sheet->resdic_item)
	{
		auto data = pair.second;

		for (auto& category : sheet->categories)
		{
			if (fs.is_open())
			{
				string str2 = data->values[category] + ',';
				fs.write(str2.c_str(), str2.size());
			}

		}
		if (fs.is_open())
		{
			fs.write(line.c_str(), line.size());
		}
	}

	fs.close();
}

void DataTableMgr::SaveSprite(string sheetName)
{
	if (resdic_sheet.find(sheetName) == resdic_sheet.end())
		return;

	fstream fs;
	fs.open(RESOURCE->directory() + '/' + "Sprite" + '/' + sheetName + ".csv", ios::out);
	if (fs.fail())
		return;

	string line = "\n";

	auto sheet = resdic_sheet[sheetName];

	// 카테고리 먼저 출력하기
	for (auto& category : sheet->categories)
	{
		if (fs.is_open())
		{
			string str = category + ',';
			fs.write(str.c_str(), str.size());
		}
	}

	if (fs.is_open())
	{
		fs.write(line.c_str(), line.size());
	}

	for (auto& pair : sheet->resdic_item)
	{
		auto data = pair.second;

		for (auto& category : sheet->categories)
		{
			if (fs.is_open())
			{
				string str2 = data->values[category] + ',';
				fs.write(str2.c_str(), str2.size());
			}

		}
		if (fs.is_open())
		{
			fs.write(line.c_str(), line.size());
		}
	}

	fs.close();
}

void DataTableMgr::SaveEffect(string sheetName)
{
	if (resdic_sheet.find(sheetName) == resdic_sheet.end())
		return;

	fstream fs;
	fs.open(RESOURCE->directory() + '/' + "Effect" + '/' + sheetName + ".csv", ios::out);
	if (fs.fail())
		return;

	string line = "\n";

	auto sheet = resdic_sheet[sheetName];

	// 카테고리 먼저 출력하기
	for (auto& category : sheet->categories)
	{
		if (fs.is_open())
		{
			string str = category + ',';
			fs.write(str.c_str(), str.size());
		}
	}

	if (fs.is_open())
	{
		fs.write(line.c_str(), line.size());
	}

	for (auto& pair : sheet->resdic_item)
	{
		auto data = pair.second;

		for (auto& category : sheet->categories)
		{
			if (fs.is_open())
			{
				string str2 = data->values[category] + ',';
				fs.write(str2.c_str(), str2.size());
			}

		}
		if (fs.is_open())
		{
			fs.write(line.c_str(), line.size());
		}
	}

	fs.close();
}

void DataTableMgr::SaveAll()
{
	for (auto sheet : resdic_sheet)
	{
		SaveSheetFile(sheet.first);
	}
}



	
