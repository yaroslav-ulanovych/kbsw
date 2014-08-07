#include "hooks.h"
#include "kbsw.h"

void DLL_EXPORT MakeSureLibraryLoaded() {}

UINT WM_CHANGELAYOUT;

LRESULT CALLBACK DLL_EXPORT CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
	CWPSTRUCT *cwp = (CWPSTRUCT*)lParam;
	if (cwp->message == WM_CHANGELAYOUT) {
		HKL hkl = (HKL)cwp->wParam;
		ActivateKeyboardLayout(hkl, 0);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			WM_CHANGELAYOUT = RegisterWindowMessage(KBSW_CHANGE_LAYOUT_MESSAGE);
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}