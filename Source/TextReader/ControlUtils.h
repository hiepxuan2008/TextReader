#pragma once
#include "TextReader.h"

class ControlUtils {
public:
	static WCHAR* getComboBoxSelectedText(HWND hwnd) {
		WCHAR* buffer = new WCHAR[MAX_LOADSTRING];
		SendMessage(hwnd, WM_GETTEXT, (WPARAM)MAX_LOADSTRING, (LPARAM)buffer);
		return buffer;
	}

	static int getComboBoxSelectedIndex(HWND hwnd) {
		return ComboBox_GetCurSel(hwnd);
	}

	static void setComboBoxSelecetedIndex(HWND hwnd, int index) {
		ComboBox_SetCurSel(hwnd, index);
	}

	static void setSliderBarSelecetedIndex(HWND hwnd, int pos) {
		SendMessage(hwnd, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)pos);
	}

	static int getSliderBarSelecetedIndex(HWND hwnd) {
		return SendMessage(hwnd, TBM_GETPOS, 0, 0);
	}
};
