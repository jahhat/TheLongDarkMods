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
#include "ModLoader_Macros.h"

// Easy access
#define MODLOADER_MAKE_FUNCTION_ACCESSIBLE() __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__))
// System Macros
#define MODLOADER_DISABLE_THREAD_CALLS(hModule, reason) if (reason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hModule)
// MirrorHook
#define MODLOADER_MIRRORHOOK_DEFINITIONS_PATH "C:/Users/berkay/source/repos/MirrorHook/MirrorHook/inc/Definitions.hpp"

MOD_NAMESPACE_BEGIN() {
   /// <summary>
   /// You probably don't want something from here.
   /// </summary>
   namespace _internal {
      typedef void(MODLOADER_API* fnExecuteInGameThread)(const std::function<void()>&);
      typedef DWORD64(MODLOADER_API* fnGetBaseAddress)(const bool);
      typedef LPVOID(MODLOADER_API* fnGetGameClassInstanceAt)(DWORD64, const bool, const bool);
   }

   /// <summary>
   /// Calls the given function pointer with the game's thread context. The functions are executed every second.
   /// </summary>
   /// <param name="pFunctionToBeCalled">A pointer to the function that will be called.</param>
   inline void MODLOADER_API ExecuteInGameThread(const std::function<void()>& functionToBeCalled) {
      reinterpret_cast<_internal::fnExecuteInGameThread>(_Notnull_ GetProcAddress(GetModuleHandle(TEXT("ModLoader.dll")), "ModLoader::Internals::ExecuteInGameThread"))(functionToBeCalled);
   }

   /// <summary>Gets the base address of GameAssembly.dll</summary>
   /// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
   /// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
   inline DWORD64 MODLOADER_API GetBaseAddress(const bool blockUntilReturn = true) {
      return reinterpret_cast<_internal::fnGetBaseAddress>(_Notnull_ GetProcAddress(GetModuleHandle(TEXT("ModLoader.dll")), "ModLoader::Internals::GetBaseAddress"))(blockUntilReturn);
   }

   /// <summary>Gets the class instance of the requested type from the given address.</summary>
   /// <param name="rva">The relative virtual address that points to the requested class instance object.</param>  
   /// <param name="addBaseAddressToRVA">Whether to add the base address from <see cref="GetBaseAddress"/> to <paramref name="rva"/>.</param>  
   /// <param name="blockUntilReturn">Whether to loop the function until the base address is returned. THIS USES <see cref="Sleep()"/>!</param>  
   /// <returns>The base address of GameAssembly.dll if successful, NULL if not.</returns>  
   /// <remarks>The function will return nullptr if <paramref name="addBaseAddressToRVA"/> is true and <see cref="GetBaseAddress"/> fails.</remarks>
   template <typename T>
   inline T* MODLOADER_API GetGameClassInstanceAt(DWORD64 rva, const bool addBaseAddressToRVA = true, const bool blockUntilReturn = true) {
      return (T*)reinterpret_cast<_internal::fnGetGameClassInstanceAt>(_Notnull_ GetProcAddress(GetModuleHandle(TEXT("ModLoader.dll")), "ModLoader::Internals::GetGameClassInstanceAt"))(rva, addBaseAddressToRVA, blockUntilReturn);
   }
} MOD_NAMESPACE_END()
