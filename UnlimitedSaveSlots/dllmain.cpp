#define WIN32_LEAN_AND_MEAN
#include <windows.h>

DWORD WINAPI Init(LPVOID) {
   DWORD64 gaBase = NULL;
   while (!gaBase) {
      gaBase = (DWORD64)GetModuleHandle(L"GameAssembly.dll");
      Sleep(1000);
   }

   DWORD64* pInst = (DWORD64*)(gaBase + 0x3C6BF40); // SaveGameSlots class instance offset
   while (!pInst || !*pInst)
      Sleep(250);

   *(DWORD*)(*(DWORD64*)(_Notnull_ *pInst + 0xB8) + 0x3C) = 999;
   return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH)
      CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Init, 0, 0, 0);
   return TRUE;
}
