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
#include <cstdint>

// ModLoader Macros
#define MODLOADER_API __fastcall
#define MODLOADER_NAMESPACE ModLoader
#define MODLOADER_NAMESPACE_BEGIN() namespace MODLOADER_NAMESPACE
#define MODLOADER_NAMESPACE_END()   // MODLOADER_NAMESPACE
#define MODLOADER_MAKE_FUNCTION_ACCESSIBLE() __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__))
// System Macros
#define MODLOADER_DISABLE_THREAD_CALLS(hModule, reason) if (reason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hModule)

MODLOADER_NAMESPACE_BEGIN() {
   namespace _internal {
      static inline DWORD64 baseAddress = NULL;
   }
   /// <summary>Gets the base address of GameAssembly.dll</summary>
   /// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
   /// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
   static inline DWORD64 MODLOADER_API GetBaseAddress(bool blockUntilReturn = true) {
      if (_internal::baseAddress)
         return _internal::baseAddress;

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

      _internal::baseAddress = reinterpret_cast<DWORD64>(gaHandle);
      return _internal::baseAddress;
   }

   /// <summary>Gets the class instance of the requested type from the given address.</summary>
   /// <param name="rvaAddress">The address/offset that points to the requested class instance object.</param>  
   /// <param name="addBaseAddressToAddress">Whether to add the base address from <see cref="GetBaseAddress"/> to <paramref name="address"/>.</param>  
   /// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
   /// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
   /// <remarks>The function will return nullptr if <paramref name="addBaseAddressToAddress"/> is true and <see cref="GetBaseAddress"/> fails.</remarks>
   template <typename T>
   static inline T* GetGameClassInstanceAt(DWORD64 rvaAddress, const bool addBaseAddressToAddress = true, bool blockUntilReturn = true) {
      if (!rvaAddress)
         return nullptr;

      rvaAddress += (addBaseAddressToAddress ? GetBaseAddress(blockUntilReturn) : 0);
      if (!*(DWORD64*)rvaAddress)
         return nullptr;

      T** ppClassInstance = reinterpret_cast<T**>(*(DWORD64*)rvaAddress + 0xB8);
      if (ppClassInstance && !*ppClassInstance)
         return *ppClassInstance;

      if (blockUntilReturn) {
         for (;;) {
            if (ppClassInstance && !*ppClassInstance)
               return *ppClassInstance;
            Sleep(250);
         }
      }
      return nullptr;
   }
} MODLOADER_NAMESPACE_END()
