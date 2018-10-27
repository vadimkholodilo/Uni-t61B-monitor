/**
 *  Product:        Tolk
 *  File:           ScreenReaderDriverZT.h
 *  Description:    Driver for the ZoomText screen reader.
 *  Copyright:      (c) 2014, Davy Kager <mail@davykager.nl>
 *  License:        LGPLv3
 */

#ifndef _SCREEN_READER_DRIVER_ZT_H_
#define _SCREEN_READER_DRIVER_ZT_H_

#include "zt.h"
#include "ScreenReaderDriver.h"

class ScreenReaderDriverZT : public ScreenReaderDriver {
public:
  ScreenReaderDriverZT();
  ~ScreenReaderDriverZT();

public:
  bool Speak(const wchar_t *str, bool interrupt);
  bool Braille(const wchar_t *) { return false; }
  bool IsSpeaking();
  bool Silence();
  bool IsActive();
  bool Output(const wchar_t *str, bool interrupt) { return Speak(str, interrupt); }

private:
  void Initialize();
  void Finalize();
  bool IsRunning() { return (!!FindWindow(L"ZXSPEECHWNDCLASS", L"ZoomText Speech Processor")); }

private:
  IZoomText2 *controller;
  ISpeech2 *speech;
};

#endif // _SCREEN_READER_DRIVER_ZT_H_
