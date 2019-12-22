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

#define MIRRORHOOK_DEFINITIONS_PATH "C:\Users\berkay\source\repos\MirrorHook\MirrorHook\inc\Definitions.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../Helpers/Memory.hpp"
#include <d3d11.h>
#include MIRRORHOOK_DEFINITIONS_PATH

DWORD WINAPI _gameThread(LPVOID) {
   for (;;) {
      if (GetAsyncKeyState(VK_F1) & 0x8000) {
         OutputDebugString(L"I see what you did there...");
      }
      if (GetAsyncKeyState(VK_F2) & 0x8000) {
         DWORD64* pInst = (DWORD64*)(Memory::baseAddress + 0x3C1E218); // InterfaceManager class instance offset
         DWORD64* pInst_RealInstance = (DWORD64*)(_Notnull_ *pInst + 0xB8);
         DWORD64* pInst_Panel_Debug = (DWORD64*)(_Notnull_ *pInst_RealInstance + 0xE8);

         //reinterpret_cast<void(__vectorcall*)(DWORD64 _this)>(gaBase + 0x98C1E0)(*pInst_Panel_Debug); // Panel_Debug::Start -> crashes on GenerateLocation* calls (game bug)
         reinterpret_cast<void(__fastcall*)(DWORD64 _this, bool)>(Memory::baseAddress + 0x98EA60)(*pInst_Panel_Debug, true); // Panel_Debug::Enable(bool)
      }
      static bool setUp = false;
      if (!setUp && GetAsyncKeyState(VK_F3) & 0x8000) {
         setUp = true;
         DWORD64 il2cppstr_uConsole = reinterpret_cast<DWORD64(__fastcall*)(const char* str)>(Memory::baseAddress + 0x282550)("uconsole"); // il2cpp_string_new_len
         DWORD64 uConsoleResource   = reinterpret_cast<DWORD64(__fastcall*)(DWORD64)>(Memory::baseAddress + 0x22B3A40)(il2cppstr_uConsole); // Resources.Load -> crashes on uConsole specifically
         //DWORD64 uConsoleObject     = reinterpret_cast<DWORD64(__vectorcall*)(DWORD64)>(gaBase + 0x224A620)(uConsoleResource); // Object.Instantiate
      }
      if (GetAsyncKeyState(VK_F5) & 0x8000) {
         reinterpret_cast<void(__fastcall*)()>(Memory::baseAddress + 0x3BB410)(); // Panel_Debug::Enable(bool)
      }

      Sleep(1000);
   }
   return FALSE;
}

#pragma optimize( "", off )
void asmProxyRemoteThread() {
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
#pragma optimize( "", on)

DWORD WINAPI Init(LPVOID) {
   Memory::Init();

   Memory::writeRaw((DWORD64)asmProxyRemoteThread, true, 7, 0x45, 0x33, 0xC9, 0x4C, 0x8B, 0xC7, 0xC3);
   Memory::writeJMP((DWORD64)hkInstantiateInterfaceObjects + 0x2D, true, (DWORD64)asmProxyRemoteThread, true);
   Memory::writeCall(0x712166, false, (DWORD64)hkInstantiateInterfaceObjects, true);
   Memory::writeNOP(0x712166 + 0x5, 1);

   MirrorHook::PrepareFor(MirrorHook::Game::UniversalD3D11, L"TheLongDark");
   return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH)
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, 0);
   return TRUE;
}