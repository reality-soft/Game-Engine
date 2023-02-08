#pragma once
#include "common.h"
#include "DllMacro.h"

namespace KGCA41B
{
	class DLL_API DataMgr
	{
		SINGLETON(DataMgr)
#define DATA DataMgr::GetInst()
	private:
		string directory_;
	public:
		string directory() { return directory_; }
		void set_directory(string dir) { directory_ = dir; }
	private:
		std::map<std::string, OpenXLSX::XLDocument*> resdic_data;
	public:
		bool Init(string directory);
		void Release();
	public:
		void CreateDocument();
		void ReadData(string data_name);
	};

}