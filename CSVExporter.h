#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
class CSVExporter
{
public:
	CSVExporter(std::wstring fileName = std::wstring(L""), std::vector<std::wstring> columns = std::vector<std::wstring>(), std::wstring delimeter = L";");
	int insert(bool time, std::vector<std::wstring> data);
	~CSVExporter();
private:
	std::wstring _delimeter;
	std::wstring _fileName;
};

