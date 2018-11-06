#include <ctime>
#include "tolk/tolk.h"
#include "decoder.h"
#include <string>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "serial/serial.h"
#pragma comment(lib, "serial.lib")
#pragma comment(lib, "Tolk.lib")
using namespace std;
using namespace serial;

void mySleep(unsigned long milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds); // 100 ms
#else
	usleep(milliseconds * 1000); // 100 ms
#endif
}

int main() {
    setlocale (LC_ALL,"russian");
	time_t speakInterval = 2;
	time_t prevTime = 0;
bool screenReaderIsRunning = false;
	Tolk_Load();
	Tolk_DetectScreenReader();
	Tolk_TrySAPI(true);
	screenReaderIsRunning = Tolk_HasSpeech();
	if (screenReaderIsRunning) {
		Tolk_Output(L"Speech is enabled");
			}
	int errorCode = 0;
	vector<serial::PortInfo> ports = serial::list_ports();
	if (!ports.size()) {
		cout << "No connected devices" << endl;
		system("pause");
		return -3;
	}
	int index = 0;
	cout << "Choose your port" << endl;
	for (int i = 0; i < ports.size(); i++) {
		cout << ports[i].port << "    " << i << endl;
	}
	cout << "Enter your choice or enter 0 and the first port in the list will be chosen." << endl;
	cin >> index;
	if (index > ports.size() - 1) cout << "Invalid index. The first port in the list will be chosen" << endl;
	string port = ports[index].port;
	wstring prevMode(L"");
	wstringstream message;
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
			if (prevMode != decoder.mode) {
				prevMode = decoder.mode;
				if (screenReaderIsRunning) Tolk_Output(decoder.mode.c_str(), true);
				else wcout << decoder.mode << endl;
			}
			if (prevValue != decoder.value && time(NULL) - prevTime >= speakInterval) {
				prevValue = decoder.value;
				if (decoder.mode.find(L"voltage") && !decoder.mode.find(L"m") && !decoder.mode.find(L"AC") || decoder.mode.find(L"Freequancy")) decoder.value *= 10;
				message = wstringstream();
				message << decoder.value << decoder.unit;
				if (screenReaderIsRunning) Tolk_Output(message.str().c_str());
				else wcout << message.str() << endl;
				prevTime = time(NULL);	}
	}
		else continue;
		mySleep(400);
		}
		system("pause");
		return 0;
	}
