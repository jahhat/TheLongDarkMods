/*
   MIT License

   Copyright (c) 2019 Berkay Yigit <berkay2578@gmail.com>
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

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <functional> // std::function
#include <vector>
#include "inc/ModLoader_Macros.h"

// Extension Macros
#define _STR(x) #x
#define _STRINGIFY(x) _STR(x)
#define ExportedFunction comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

MODLOADER_NAMESPACE_BEGIN() {
   namespace Internals {
      typedef void(MODLOADER_API* fnNoParam)();
      typedef void(MODLOADER_API* fnOneBool)(bool);

      DWORD64 baseAddress = NULL;
      auto    vFuncQueue  = std::vector<std::function<void()>>();

      void    MODLOADER_API ExecuteInGameThread(const std::function<void()>& functionToBeCalled) {
      #pragma ExportedFunction
         vFuncQueue.push_back(functionToBeCalled);
      }
      DWORD64 MODLOADER_API GetBaseAddress(const bool blockUntilReturn) {
      #pragma ExportedFunction
         if (baseAddress)
            return baseAddress;

         HMODULE gaHandle = GetModuleHandleW(L"GameAssembly.dll");
         if (blockUntilReturn) {
            while (!gaHandle) {
               gaHandle = GetModuleHandleW(L"GameAssembly.dll");
               Sleep(100);
            }
         } else {
            if (!gaHandle)
               return NULL;
         }

         baseAddress = (DWORD64)gaHandle;
         return baseAddress;
      }
      LPVOID  MODLOADER_API GetGameClassInstanceAt(DWORD64 rva, const bool addBaseAddressToRVA, const bool blockUntilReturn) {
      #pragma ExportedFunction
         if (!rva)
            return nullptr;

         rva += (addBaseAddressToRVA ? GetBaseAddress(blockUntilReturn) : 0);
         if (!*(DWORD64*)rva) {
            if (blockUntilReturn) {
               while (!*(DWORD64*)rva)
                  Sleep(250);
            } else {
               return nullptr;
            }
         }

         LPVOID* ppClassInstance = reinterpret_cast<LPVOID*>(*(DWORD64*)rva + 0xB8);
         if (ppClassInstance && *ppClassInstance)
            return *ppClassInstance;

         if (blockUntilReturn) {
            while (!ppClassInstance && _Notnull_ !*ppClassInstance)
               Sleep(250);
            return *ppClassInstance;
         }
         return nullptr;
      }
      
      void hkGameManager_Update() {
         if (!vFuncQueue.empty()) {
            for (auto& func : vFuncQueue)
               func();
            vFuncQueue.clear();
         }
      }
      void hkGameManager_Update_ASMProxy() {
         hkGameManager_Update();
         hkGameManager_Update();
         hkGameManager_Update();
      }
   }
} MODLOADER_NAMESPACE_END()