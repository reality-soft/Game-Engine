#include "DataMgr.h"

using namespace KGCA41B;
using namespace OpenXLSX;

bool KGCA41B::DataMgr::Init(string directory)
{
	set_directory(directory);

	return true;
}

void KGCA41B::DataMgr::Release()
{
	for (auto& pair : resdic_data)
	{
		pair.second->save();
		pair.second->close();
		delete pair.second;
	}
}

void DataMgr::CreateDocument()
{
	XLDocument doc;
	doc.create("Spreadsheet.xlsx");
	auto wks = doc.workbook().worksheet("Sheet1");

	wks.cell("A1").value() = "Hello, OpenXLSX!";

	doc.save();

	return;
}

void DataMgr::ReadData(string data_name)
{
	auto path = directory_ + "/" + data_name;
	XLDocument* newData = new XLDocument(path);

	resdic_data.insert({ data_name, newData });
}
