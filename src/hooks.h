#include <windows.h>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

void DLL_EXPORT MakeSureLibraryLoaded();
LRESULT CALLBACK DLL_EXPORT CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
