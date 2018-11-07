#include "CSVExporter.h"
#include <ctime>


CSVExporter::CSVExporter(std::wstring fileName, std::vector<std::wstring> columns, std::wstring delimeter)
{
	if (fileName != L"") {
		_fileName = fileName;
		_delimeter = delimeter;
		insert(false, columns);
	}
	}
int CSVExporter::insert(bool insertTime, std::vector<std::wstring> data)
{
	std::wofstream log(_fileName, std::ios::app);
	if (!log) return -1;
	if (insertTime) {
		char timeBuffer[9];
		const time_t seconds = time(NULL);
		tm* ltime = localtime(&seconds);
			strftime(timeBuffer, 9, "%H:%M:%S", ltime);
		log << timeBuffer << _delimeter << L" ";
	}
	for (auto column : data) {
		log << column << _delimeter;;
	}
	log << L"\n";
	log.close();
	return 0;
}


CSVExporter::~CSVExporter()
{
}
