/*
   MIT License

   Original: Copyright (c) 2018 FINDarkside
   Modifications: Copyright (c) 2019 Berkay Yigit <berkay2578@gmail.com>
       Copyright holder detail: Nickname(s) used by the copyright holder: 'berkay2578', 'berkayylmao'.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../Helpers/Memory.hpp"

DWORD64 gaBase = NULL;
DWORD WINAPI _gameThread(LPVOID) {
   // il2cpp_thread_attach
   reinterpret_cast<void(__vectorcall*)(LPVOID)>(gaBase + 0x282720)(
      // il2cpp_domain_get
      reinterpret_cast<LPVOID(__vectorcall*)()>(gaBase + 0x2816A0)()
      );

   for (;;) {
      if (GetAsyncKeyState(VK_F1) & 0x8000) {
         OutputDebugString(L"I see what you did there...");
      }
      if (GetAsyncKeyState(VK_F2) & 0x8000) {
         DWORD64* pInst = (DWORD64*)(gaBase + 0x3C1E218); // InterfaceManager class instance offset
         DWORD64* pInst_RealInstance = (DWORD64*)(_Notnull_ *pInst + 0xB8);
         DWORD64* pInst_Panel_Debug = (DWORD64*)(_Notnull_ *pInst_RealInstance + 0xE8);

         //reinterpret_cast<void(__vectorcall*)(DWORD64 _this)>(gaBase + 0x98C1E0)(*pInst_Panel_Debug); // Panel_Debug::Start -> crashes on GenerateLocation* calls (game bug)
         reinterpret_cast<void(__vectorcall*)(DWORD64 _this, bool)>(gaBase + 0x98EA60)(*pInst_Panel_Debug, true); // Panel_Debug::Enable(bool)
      }
      static bool setUp = false;
      if (!setUp && GetAsyncKeyState(VK_F3) & 0x8000) {
         setUp = true;
         DWORD64 il2cppstr_uConsole = reinterpret_cast<DWORD64(__vectorcall*)(const char* str)>(gaBase + 0x282550)("uConsole"); // il2cpp_string_new_len
         DWORD64 uConsoleResource   = reinterpret_cast<DWORD64(__vectorcall*)(DWORD64)>(gaBase + 0x22B3A40)(il2cppstr_uConsole); // Resources.Load
         //DWORD64 uConsoleObject     = reinterpret_cast<DWORD64(__vectorcall*)(DWORD64)>(gaBase + 0x224A620)(uConsoleResource); // Object.Instantiate
      }
      Sleep(1000);
   }
   return FALSE;
}

#pragma optimize( "", off )
void asmProxy() {
   DWORD64 ff = 9999999999999;
   DWORD64 zz = ff / 2;
   while (zz > ff) {
      ff = zz;
      zz = ff / 2;
   }
}

void hkInstantiateInterfaceObjects() {
   CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&_gameThread, NULL, 0, 0);
   // +0x2D
}

bool setUp = false;
void hkGameManager_Update() {
   if (GetAsyncKeyState(VK_F1) & 0x8000) {
      OutputDebugString(L"I see what you did there...");
   }
   if (GetAsyncKeyState(VK_F2) & 0x8000) {
      DWORD64* pInst = (DWORD64*)(gaBase + 0x3C1E218); // InterfaceManager class instance offset
      DWORD64* pInst_RealInstance = (DWORD64*)(_Notnull_ *pInst + 0xB8);
      DWORD64* pInst_Panel_Debug = (DWORD64*)(_Notnull_ *pInst_RealInstance + 0xE8);

      //reinterpret_cast<void(__vectorcall*)(DWORD64 _this)>(gaBase + 0x98C1E0)(*pInst_Panel_Debug); // Panel_Debug::Start -> crashes on GenerateLocation* calls (game bug)
      reinterpret_cast<void(__vectorcall*)(DWORD64 _this, bool)>(gaBase + 0x98EA60)(*pInst_Panel_Debug, true); // Panel_Debug::Enable(bool)
   }
   if (!setUp && GetAsyncKeyState(VK_F3) & 0x8000) {
      setUp = true;
      DWORD64 il2cppstr_uConsole = reinterpret_cast<DWORD64(__vectorcall*)(const char* str)>(gaBase + 0x282550)("uConsole"); // il2cpp_string_new_len
      DWORD64 uConsoleResource   = reinterpret_cast<DWORD64(__vectorcall*)(DWORD64, DWORD64)>(gaBase + 0x22B3A40)(il2cppstr_uConsole, NULL); // Resources.Load
      //DWORD64 uConsoleObject     = reinterpret_cast<DWORD64(__vectorcall*)(DWORD64)>(gaBase + 0x224A620)(uConsoleResource); // Object.Instantiate
   }
}
#pragma optimize( "", on)

DWORD WINAPI Init(LPVOID) {
   Memory::Init();
   while (!gaBase) {
      gaBase = (DWORD64)GetModuleHandle(L"GameAssembly.dll");
      Sleep(1000);
   }

   //reinterpret_cast<void(__vectorcall*)(LPVOID)>(gaBase + 0x282720)( // il2cpp_thread_attachk
   //   reinterpret_cast<LPVOID(__vectorcall*)()>(gaBase + 0x2816A0)() // il2cpp_domain_get
   //);

   Memory::writeRaw((DWORD64)asmProxy, 7, 0x45, 0x33, 0xC9, 0x4C, 0x8B, 0xC7, 0xC3);
   Memory::writeJMP((DWORD64)hkInstantiateInterfaceObjects + 0x2D, (DWORD64)asmProxy);
   Memory::writeCall(gaBase + 0x712166, (DWORD64)hkInstantiateInterfaceObjects);
   Memory::writeNOP(gaBase + 0x712166 + 0x5, 1);

   // // Hook GameManager::Update -> ret
   //Memory::writeRaw((DWORD64)asmProxy, 5, 0x41, 0x5E, 0x5F, 0x5D, 0xC3);
   //Memory::writeJMP((DWORD64)hkGameManager_Update + 0xED, (DWORD64)asmProxy);
   //Memory::writeJMP(gaBase + 0x36FD92, (DWORD64)hkGameManager_Update);

   return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH) {
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, 0);
   }
   return TRUE;
}