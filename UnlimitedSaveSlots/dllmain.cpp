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
#include <windows.h>
#include "../ModLoader/inc/ModLoader.hpp"
#include "../ModLoader/inc/GameTypes/SaveGameSlots.hpp"

namespace ModLoader {
   void MODLOADER_API OnLoad() {
      MODLOADER_MAKE_FUNCTION_ACCESSIBLE();
      auto* pSaveGameSlots = ModLoader::GetGameClassInstanceAt<SaveGameSlots>(SaveGameSlots::_ClassInstanceOffset);
      pSaveGameSlots->MAX_SAVESLOTS = 999;
   }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   MODLOADER_DISABLE_THREAD_CALLS(hModule, reason);
   return TRUE;
}
