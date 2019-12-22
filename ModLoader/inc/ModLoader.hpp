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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

// ModLoader Macros
#define MODLOADER_API __fastcall
#define MODLOADER_NAMESPACE ModLoader
#define MODLOADER_NAMESPACE_BEGIN() namespace MODLOADER_NAMESPACE {
#define MODLOADER_NAMESPACE_END()   } // MODLOADER_NAMESPACE
#define MODLOADER_MAKE_FUNCTION_ACCESSIBLE() __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__))

// System Macros
#define MODLOADER_DISABLE_THREAD_CALLS(hModule, reason) if (reason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hModule)

MODLOADER_NAMESPACE_BEGIN()
/// <summary>Gets the base address of GameAssembly.dll</summary>
/// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
/// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
DWORD64 MODLOADER_API GetBaseAddress(bool blockUntilReturn = true) {
   HMODULE gaHandle = GetModuleHandleW(L"GameAssembly.dll");
   if (blockUntilReturn) {

   } else {
      if (!gaHandle)
         return NULL;
   }
   return reinterpret_cast<DWORD64>(gaHandle);
}

/// <summary>Gets the class instance of the requested type from the given address.</summary>
/// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
/// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
template <typename T>
T* GetGameClassInstanceAt(DWORD64 address, const bool addBaseAddressToAddress = true) {
   if (!address)
      return nullptr;

   address += addBaseAddressToAddress;
   if (!*address)
      return nullptr;

   T** ppClassInstance = reinterpret_cast<T**>(*address + 0xB8);
   if (ppClassInstance && !*ppClassInstance)
      return *ppClassInstance;

   return nullptr;
}
MODLOADER_NAMESPACE_END()
