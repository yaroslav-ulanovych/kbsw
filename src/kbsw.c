#if defined(UNICODE) && !defined(_UNICODE)
	#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
	#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "hooks.h"
#include "kbsw.h"

#define TRAY_ICON_EVENT WM_APP + 1
#define MENU_ITEM_ID_EXIT 2000
#define TRAY_ICON_ID_MAIN 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HMODULE hHooksMod;
HKL layouts[64];
UINT WM_CHANGELAYOUT;
HINSTANCE hInstance;
HWND hWnd;
HICON hTrayIcon;
HHOOK hLowLevelKeyboardHook;
HHOOK hCallWndHook;
DWORD prevKeyDownCode;
DWORD prevKeyDownTime;

TCHAR szClassName[ ] = _T("kbswWindowClassName");

void LoadTrayIcon() {
	hTrayIcon = LoadIcon(hInstance, "TRAYICON");
	printf("hTrayIcon: %u\n", hTrayIcon);
}

void AddTrayIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID_MAIN;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = TRAY_ICON_EVENT;
	nid.hIcon = hTrayIcon;
	strcpy(nid.szTip, "kbsw");
	BOOL res = Shell_NotifyIcon(NIM_ADD, &nid);
	printf("add tray icon: %s\n", res ? "OK" : "ERROR");
}

void RemoveTrayIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID_MAIN;
	BOOL res = Shell_NotifyIcon(NIM_DELETE, &nid);
	printf("remove tray icon: %s\n", res ? "OK" : "ERROR");
}

void InstallHooks() {
	hLowLevelKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	printf("install LowLevelKeyboardHook: %s\n", hLowLevelKeyboardHook != NULL ? "OK" : "ERROR");
	hCallWndHook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, hHooksMod, 0);
	printf("install CallWndHook: %s\n", hCallWndHook != NULL ? "OK" : "ERROR");
}

void UninstallHooks() {
	int res;
	res = UnhookWindowsHookEx(hLowLevelKeyboardHook);
	printf("uninstall LowLevelKeyboardHook: %s\n", res != 0 ? "OK" : "ERROR");
	res = UnhookWindowsHookEx(hCallWndHook);
	printf("uninstall CallWndHook: %s\n", res != 0 ? "OK" : "ERROR");
}

void ShowPopupMenu() {
	SetForegroundWindow(hWnd);
	HMENU hMenu = CreatePopupMenu();
	InsertMenu(hMenu, -1, MF_BYPOSITION | MF_STRING, MENU_ITEM_ID_EXIT, "Exit");
	POINT pt;
	GetCursorPos(&pt);
	WORD cmd = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hWnd, NULL);
	SendMessage(hWnd, WM_COMMAND, cmd, 0);
	DestroyMenu(hMenu);
}

void LoadHooksLibrary() {
	MakeSureLibraryLoaded();
	hHooksMod = GetModuleHandle("hooks");
	printf("hooks module handle: %u\n", hHooksMod);
}

void LoadLayouts() {
	int x = GetKeyboardLayoutList(64, layouts);
	printf("layouts found: %i\n", x);
	printf("layout 1: %x\n", layouts[0]);
	printf("layout 2: %x\n", layouts[1]);
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	hInstance = hThisInstance;
	WNDCLASSEX wincl;

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WndProc;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if (!RegisterClassEx (&wincl))
		return 0;

	hWnd = CreateWindowEx(0, szClassName, _T("kbsw window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 544, 375, HWND_DESKTOP, NULL, hInstance, NULL);

	//ShowWindow (hWnd, nCmdShow);

	MSG msg;
	while (GetMessage (&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void changeLayout(HKL layout) {
	HWND hWnd = GetForegroundWindow();
	PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)layout);
	SendNotifyMessage(hWnd, WM_CHANGELAYOUT, (WPARAM)layout, 0);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT *kdhs = (KBDLLHOOKSTRUCT*)lParam;
		DWORD vkCode = kdhs->vkCode;
		DWORD time = kdhs->time;
		switch(wParam) {
			case WM_KEYDOWN:
				if (vkCode != prevKeyDownCode) {
					prevKeyDownCode = vkCode;
					prevKeyDownTime = time;
				}
				break;
			case WM_KEYUP:
				if ((vkCode == prevKeyDownCode) && (time - prevKeyDownTime < 200)) {
					switch(vkCode) {
						case VK_LCONTROL:
							// printf("control\n");
							changeLayout(layouts[1]);
							break;
						case VK_LSHIFT:
							// printf("shift\n");
							changeLayout(layouts[0]);
							break;
					}
				}
				prevKeyDownCode = 0;
				prevKeyDownTime = 0;
				break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE:
			WM_CHANGELAYOUT = RegisterWindowMessage(KBSW_CHANGE_LAYOUT_MESSAGE);
			LoadLayouts();
			LoadTrayIcon();
			AddTrayIcon(hWnd);
			LoadHooksLibrary();
			InstallHooks();
			break;
		case WM_CLOSE:
			UninstallHooks();
			RemoveTrayIcon(hWnd);
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case MENU_ITEM_ID_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			break;
		case TRAY_ICON_EVENT:
			switch (lParam) {
				case WM_RBUTTONUP:
					ShowPopupMenu();
				break;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}