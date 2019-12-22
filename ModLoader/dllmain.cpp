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

#define MIRRORHOOK_DEFINITIONS_PATH "C:\Users\berkay\source\repos\MirrorHook\MirrorHook\inc\Definitions.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <string>
#include <thread>
#include MIRRORHOOK_DEFINITIONS_PATH

HMODULE hThisModule = NULL;
const std::string szModsFolderName = "Mods";

DWORD WINAPI Init(LPVOID) {
   wchar_t szGameDir[MAX_PATH] ={ 0 };
   // Get TLD's directory, for some reason 'std::filesystem::current_path()' returns the root directory
   {
      if (!GetModuleFileNameW(hThisModule, szGameDir, _countof(szGameDir))) {
         std::wstring msg = L"GetModuleFileNameW failed.\n\nError:\n" + std::to_wstring(GetLastError());
         MessageBoxW(NULL, msg.c_str(), L"TLD ModLoader - ERROR", MB_ICONERROR);

         return FALSE;
      }
   }

   bool  loadedMirrorHook = false;
   auto  loadedMods       = std::vector<HMODULE>();
   auto& gameDir          = std::filesystem::path(szGameDir).parent_path();
   auto& modsDir          = gameDir / szModsFolderName;

   if (!std::filesystem::exists(gameDir / L"MirrorHook.dll")) {
      MessageBoxW(NULL, L"MirrorHook.dll is missing. Some mods may not work/game might be unstable.", L"TLD ModLoader - WARNING", MB_ICONWARNING);
   } else {
      if (!LoadLibraryW((gameDir / L"MirrorHook.dll").c_str())) {
         MessageBoxW(NULL, L"MirrorHook could not be loaded. Some mods may not work and game might be unstable.", L"TLD ModLoader - WARNING", MB_ICONWARNING);
      } else {
         loadedMirrorHook = true;
      }
   }

   if (std::filesystem::exists(modsDir) && std::filesystem::is_directory(modsDir)) {
      SetCurrentDirectoryW(modsDir.c_str());

      for (auto& file : std::filesystem::recursive_directory_iterator(modsDir)) {
         if (!std::filesystem::is_regular_file(file)) // is not a link, directory or pipe
            continue;
         if (file.path().extension().string() != ".dll") // is a dll file
            continue;
         if (GetModuleHandleW(file.path().c_str())) // is not already loaded
            continue;

         auto& modAbsolutePath = modsDir / file.path().filename();
         auto  handle = LoadLibraryW(modAbsolutePath.c_str());
         SetCurrentDirectoryW(modsDir.c_str()); // mod may change this

         if (!handle) {
            auto e = GetLastError();
            if (e != ERROR_DLL_INIT_FAILED) {
               std::wstring msg = L"Unable to load " + modAbsolutePath.wstring() + L".\n\nError:\n" + std::to_wstring(e);
               MessageBoxW(NULL, msg.c_str(), L"TLD ModLoader - ERROR", MB_ICONERROR);
            }
         } else {
            loadedMods.push_back(handle);
         }
      }
   }
   // TODO: require config.json from modders, do priority listing
   for (auto& modDLLHandle : loadedMods) {
      if (auto fnOnLoad = GetProcAddress(modDLLHandle, "ModLoader::OnLoad"))
         std::thread(reinterpret_cast<void(__stdcall*)()>(fnOnLoad)).detach();
   }

   if (loadedMirrorHook) {
      HWND windowHandle = FindWindowW(NULL, L"TheLongDark");
      while (!windowHandle) {
         windowHandle = FindWindowW(NULL, L"TheLongDark");
         Sleep(100);
      }

      MirrorHook::PrepareFor(MirrorHook::Game::UniversalD3D11, L"TheLongDark");
   }

   return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH) {
      hThisModule = hModule;
      DisableThreadLibraryCalls(hModule);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, 0);
   }
   return TRUE;
}

