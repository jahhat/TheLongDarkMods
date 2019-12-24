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
#include <windef.h>
#include <cstdarg>
#include <cstdint>

namespace Memory {
   static CRITICAL_SECTION cs;
   static DWORD64 baseAddress, memoryAccessAddress;
   static DWORD   oldMemoryAccess;
   static int32_t memoryAccessSize;

   static void openMemoryAccess(const DWORD64 address, const bool isAddressAbsolute, const int32_t size) {
      memoryAccessAddress = address + (isAddressAbsolute ? 0 : baseAddress);
      memoryAccessSize = size;
      VirtualProtect((LPVOID)memoryAccessAddress, size, PAGE_EXECUTE_READWRITE, &oldMemoryAccess);
   }
   static void restoreMemoryAccess() {
      VirtualProtect((LPVOID)memoryAccessAddress, memoryAccessSize, oldMemoryAccess, &oldMemoryAccess);
      memoryAccessAddress = 0;
      memoryAccessSize = 0;
   }

   static DWORD32 calculateRelativeAddress(const DWORD64 from, const DWORD64 to) {
      return (DWORD32)(to - from - 0x5);
   }

   static void writeRaw(const DWORD64 to, const bool isToAbsolute, const int32_t byteCount, ...) {
      DWORD64 addr = to + (isToAbsolute ? 0 : baseAddress);

      EnterCriticalSection(&cs);
      openMemoryAccess(addr, true, byteCount);

      va_list bytes;
      va_start(bytes, byteCount);
      for (int32_t i = 0; i < byteCount; i++)
         *(BYTE*)(addr + i) = va_arg(bytes, BYTE);
      va_end(bytes);

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   static void writeCall(const DWORD64 from, const bool isFromAbsolute, const DWORD64 to, const bool isToAbsolute) {
      DWORD64 addrFrom = from + (isFromAbsolute ? 0 : baseAddress);
      DWORD64 addrTo   = to + (isToAbsolute ? 0 : baseAddress);

      EnterCriticalSection(&cs);
      openMemoryAccess(addrFrom, true, 5);

      *(BYTE*)(addrFrom) = 0xE8;
      *(DWORD32*)(addrFrom + 0x1) = calculateRelativeAddress(addrFrom, addrTo);

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   static void writeJMP(const DWORD64 from, const bool isFromAbsolute, const DWORD64 to, const bool isToAbsolute) {
      DWORD64 addrFrom = from + (isFromAbsolute ? 0 : baseAddress);
      DWORD64 addrTo   = to + (isToAbsolute ? 0 : baseAddress);

      EnterCriticalSection(&cs);
      openMemoryAccess(addrFrom, true, 5);

      *(BYTE*)(addrFrom) = 0xE9;
      *(DWORD32*)(addrFrom + 0x1) = calculateRelativeAddress(addrFrom, addrTo);

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   static void writeNOP(const DWORD64 to, const int32_t amount, const bool isToAbsolute = false) {
      DWORD64 addr = to + (isToAbsolute ? 0 : baseAddress);

      EnterCriticalSection(&cs);
      openMemoryAccess(addr, true, amount);

      for (int32_t i = 0; i < amount; i++)
         *(BYTE*)(addr + i) = 0x90;

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   static void Init() {
      InitializeCriticalSection(&cs);

      while (!baseAddress) {
         baseAddress = (DWORD64)GetModuleHandle(L"GameAssembly.dll");
         Sleep(100);
      }
   }
}