#include <iostream>
#include <cstring>
#include <string>
#include <bitset>

	// flags
	enum FLAGS {
		FLAGS_BPN,
		FLAGS_HOLD,
		FLAGS_REL,
		FLAGS_AC,
		FLAGS_DC,
		FLAGS_AUTO,
		FLAGS_EMPTY_BIT6,
		FLAGS_EMPTY_BIT_7,
		FLAGS_Z3,
		FLAGS_n,
		FLAGS_BAT,
		FLAGS_APO,
		FLAGS_MIN,
		FLAGS_MAX,
		FLAGS_Z2,
		FLAGS_Z1,
		FLAGS_Z4,
		FLAGS_PERCENT,
		FLAGS_DIOD,
		FLAGS_BEEP,
		FLAGS_M,
		FLAGS_k,
		FLAGS_m,
		FLAGS_MICRO,
		FLAGS_DEGREES_F,
		FLAGS_DEGREES_C,
		FLAGS_F,
		FLAGS_Hz,
		FLAGS_HFE,
		FLAGS_Ohm,
		FLAGS_A,
		FLAGS_V,
	};
class FS922Decoder {
    public:
    std::wstring mode;
    float value;
    std::wstring unit;
	float multiplier;
    FS922Decoder() {
        mode  = L"Unknown";
        value = 0.0;
        unit = L"";
		multiplier = 1.0;
    }
    
	int decodeData(const uint8_t* data) {
		mode = L"";
		unit = L"";
		multiplier = 1.0;
		unsigned int rawFlags = 0;
		uint8_t sign = 0;
		float point = 0.0;
		int digits = 0;
		int barGraph = 0;
		switch (data[0]) {
		case 0x2B: sign = 1;
			break;
		case 0x2D: sign = -1;
			break;
		default: return -1;
		}
		for (int i = 1; i <= 4; i++) {
			digits *= 10;
			if (isdigit(data[i])) digits += data[i] - 0x30;
			else break;
		}
		if (data[5] != 0x20) return 5;
		switch (data[6]) {
		case 0x31: point = 1.0;
			break;
		case 0x32: point = 0.001;
			break;
		case 0x33: point = 0.01;
			break;
		case 0x34: point = 0.1;
			break;
		default: point = 0.0;
		}
				// getting flags
		rawFlags = data[10];
		for (int i = 9; i >= 7; --i) {
			rawFlags <<= 8;
			rawFlags |= data[i];
		}
		std::bitset<32>flags(rawFlags);
		barGraph = data[11];
		barGraph &= 0b111111;
		if (barGraph & 128) barGraph *= -1;
		if (data[12] != 0x0D) return 12;
		if (data[13] != 0x0A) return 13;
		value = (float)sign * (float)digits * point;
		if (flags.test(FLAGS_MICRO)) {
			mode = L"Micro";
			unit = L"Micro ";
		}
		if (flags.test(FLAGS_n)) {
			mode = L"Nano";
			unit = L"n";
		}
		if (flags.test(FLAGS_m)) {
			mode = L"Milli";
			unit = L"m";
		}
		if (flags.test(FLAGS_k)) {
			mode = L"Killo";
			unit = L"k";
		}
		if (flags.test(FLAGS_M)) {
			mode = L"Mega";
			unit = L"M";
		}
		if (flags.test(FLAGS_V)) {
			mode += L" voltage";
			unit += L"V";
			if (!flags.test(FLAGS_m) && flags.test(FLAGS_DC)) multiplier = 10.0;
		}
		if (flags.test(FLAGS_A)) {
			mode += L" amperage";
			unit += L"A";
		}
		if (flags.test(FLAGS_Ohm)) {
			mode += L" Ohm";
			unit += L"Ohm";
		}
		if (flags.test(FLAGS_F)) {
			mode += L" farads";
			unit += L"F";
		}
		if (flags.test(FLAGS_Hz)) {
			mode = L" frequancy";
			unit += L"Hz";
			multiplier = 10.0;
		}
		if (flags.test(FLAGS_BEEP)) mode = L"beep";
		if (flags.test(FLAGS_DEGREES_C)) {
			mode = L"Temperature(C)";
		}
		if (flags.test(FLAGS_DEGREES_F)) {
			mode += L"temperature(F)";
		}
		if (flags.test(FLAGS_PERCENT)) {
			mode += L"Duty cycle ";
			unit += L"%";
		}
		if (flags.test(FLAGS_DIOD)) mode = L"Diod test ";
		if (flags.test(FLAGS_DC)) mode += L" DC";
		if (flags.test(FLAGS_AC)) mode += L" AC";
		if (flags.test(FLAGS_AUTO)) mode += L" auto";
        if (flags.test(FLAGS_HOLD)) mode = L"Hold";
		if (flags.test(FLAGS_HFE)) mode = L"HFE";
		if (flags.test(FLAGS_REL)) mode = L"Relative measurement";
		return 0;
	}
};