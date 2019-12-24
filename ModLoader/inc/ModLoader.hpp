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

// Mod Macros
#define MOD_NAMESPACE Mod
#define MOD_NAMESPACE_BEGIN() namespace MOD_NAMESPACE
#define MOD_NAMESPACE_END()   // MOD_NAMESPACE
// ModLoader Macros
#define MODLOADER_API __fastcall // defaults to ms_abi
#define MODLOADER_NAMESPACE ModLoader
#define MODLOADER_NAMESPACE_BEGIN() namespace MODLOADER_NAMESPACE
#define MODLOADER_NAMESPACE_END()   // MODLOADER_NAMESPACE
#define MODLOADER_MAKE_FUNCTION_ACCESSIBLE() __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__))
// System Macros
#define MODLOADER_DISABLE_THREAD_CALLS(hModule, reason) if (reason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hModule)
// MirrorHook
#define MODLOADER_MIRRORHOOK_DEFINITIONS_PATH "C:\Users\berkay\source\repos\MirrorHook\MirrorHook\inc\Definitions.hpp"
// Extension Macros
#define _STR(x) #x
#define _STRINGIFY(x) _STR(x)

MOD_NAMESPACE_BEGIN() {
   namespace _internal {
      static inline DWORD64 baseAddress = NULL;

      typedef void(MODLOADER_API* fnOnLoad)(bool isMirrorHookLoaded);
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

      _internal::baseAddress = (DWORD64)gaHandle;
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
      if (!*(DWORD64*)rvaAddress) {
         if (blockUntilReturn) {
            while (!*(DWORD64*)rvaAddress)
               Sleep(250);
         } else {
            return nullptr;
         }
      }

      T** ppClassInstance = reinterpret_cast<T**>(*(DWORD64*)rvaAddress + 0xB8);
      if (ppClassInstance && *ppClassInstance)
         return *ppClassInstance;

      if (blockUntilReturn) {
         while (!ppClassInstance && _Notnull_ !*ppClassInstance)
            Sleep(250);
         return *ppClassInstance;
      }
      return nullptr;
   }
} MODLOADER_NAMESPACE_END()
