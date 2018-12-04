#define _CRT_SECURE_NO_WARNINGS
#include "CSVExporter.h"
#include <algorithm>
#include <ctime>
#ifdef _WIN32
#include "tolk/tolk.h"
#include <windows.h>
#pragma comment(lib, "serial.lib")
#pragma comment(lib, "Tolk.lib")
#endif
#include "decoder.h"
#include <string>
#include <sstream>
#include <iostream>
#include <clocale>
#include <cstring>
#include <cstdlib>
#include "serial/serial.h"
using namespace std;
using namespace serial;
// functions
void printHelp() {
	cout << "Useage" << endl;
	cout << "Commands: " << endl;
	cout << "--log exports data in CSV format" << endl;
	cout << "--noconfig prevents program asking COM port. The first found port will be used" << endl;
	cout << "--help - prints help" << endl;
}

void mySleep(unsigned long milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds); // 100 ms
#else
	usleep(milliseconds * 1000); // 100 ms
#endif
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "");
	cout << "Uni-t 61B monitor" << endl;
	time_t speakInterval = 2;
	time_t prevTime = 0;
	int errorCode = 0;
	bool loggerIsEnabled = false;
	vector<wstring> columns;
CSVExporter exporter;
	vector<wstring>message;
	vector<string>commandLineArguments;
if (argc > 1) {
for (int i = 1; i < argc; i++) {
			commandLineArguments.push_back(argv[i]);
		}
	}
if (find(commandLineArguments.begin(), commandLineArguments.end(), "--help") != commandLineArguments.end()) printHelp();
bool screenReaderIsRunning = false;
	Tolk_Load();
	Tolk_DetectScreenReader();
	Tolk_TrySAPI(true);
	screenReaderIsRunning = Tolk_HasSpeech();
	if (screenReaderIsRunning) {
		Tolk_Output(L"Speech is enabled");
			}
vector<serial::PortInfo> ports = serial::list_ports();
	if (!ports.size()) {
		cout << "No connected devices" << endl;
		Tolk_Output(L"No connected devices");
		system("pause");
		return -3;
	}
	int index = 0;
if (find(commandLineArguments.begin(), commandLineArguments.end(), "--noconfig") == commandLineArguments.end()) {
cout << "Choose your port" << endl;
		for (int i = 0; i < ports.size(); i++) {
			cout << ports[i].port << "    " << i << endl;
		}
		cout << "Enter your choice or enter 0 and the first port in the list will be chosen." << endl;
		cin >> index;
		if (index > ports.size() - 1) cout << "Invalid index. The first port in the list will be chosen" << endl;
	}
	if (find(commandLineArguments.begin(), commandLineArguments.end(), "--log") != commandLineArguments.end()) {
		loggerIsEnabled = true;
		columns.push_back(L"Time");
		columns.push_back(L"Mode");
		columns.push_back(L"Value");
		columns.push_back(L"Unit");
		exporter = CSVExporter(L"log.csv", columns);
	}
	string port = ports[index].port;
	wstring prevMode(L"");
	wstringstream ss;
	float prevValue = 0.0;
	Serial ser;
	ser.setPort(port);
	ser.setBaudrate(2400);
	try {
		ser.open();
	}
	catch (...) {
		cout << "Unable to open " << port << endl;
		system("pause");
		exit(2);
	}
	//Opticle pare of multimeter uses DTR and RTS as power lines, that's why they have to be set as DTR = 1 and RTS = 0
	ser.setDTR(1);
	ser.setRTS(0);
	FS922Decoder decoder = FS922Decoder();
	uint8_t rawData[14];
	// Untill the end of time
	prevTime = time(NULL);
	wstring formattedValue;
	while (true) {
		try {
			ser.read(rawData, 14);
		}
		catch (...) {
			cout << "Connection was lost" << endl;
			break;
		}
		errorCode = decoder.decodeData(rawData);
		if (!errorCode) {
			decoder.value *= decoder.multiplier;
			ss << decoder.value;
if (prevMode != decoder.mode) {
				prevMode = decoder.mode;
				if (screenReaderIsRunning) Tolk_Output(decoder.mode.c_str(), true);
				else wcout << decoder.mode << endl;
			}
			if (prevValue != decoder.value && time(NULL) - prevTime >= speakInterval) {
				prevValue = decoder.value;
								if (screenReaderIsRunning) {
                    Tolk_Output(wstring(ss.str()+decoder.unit).c_str());
                }
				else wcout << decoder.value << " " << decoder.unit << endl;
prevTime = time(NULL);
			}
			if (loggerIsEnabled) {
				message.clear();
				message.push_back(decoder.mode);
message.push_back(ss.str());
				message.push_back(decoder.unit);
				exporter.insert(true, message);
			}
			ss = wstringstream();
			ss.imbue(std::locale(""));
}
		else {
			cout << "Bit " << errorCode << " is rong" << endl;
			continue;
		}
		mySleep(400);
		}
		system("pause");
		return 0;
	}

