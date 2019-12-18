#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void UnityMain() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
   reinterpret_cast<void(WINAPI*)()>(_Notnull_ GetProcAddress(LoadLibrary(L"RealUnityPlayer.dll"), "UnityMain"))();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH)
      LoadLibrary(L"mloader.dll");
   return TRUE;
}

