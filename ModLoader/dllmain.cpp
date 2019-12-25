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
#include <filesystem>
#include <string>
#include <thread>
#include "ModLoader_Internals.hpp"
#include "../Helpers/Memory.hpp"
#include MODLOADER_MIRRORHOOK_DEFINITIONS_PATH

MODLOADER_NAMESPACE_BEGIN() {
   HMODULE hThisModule = NULL;
   const std::string strModsFolderName = "Mods";
   const std::string strFnModPre       = _STRINGIFY(MOD_NAMESPACE) + std::string("::Pre");
   const std::string strFnModOnload    = _STRINGIFY(MOD_NAMESPACE) + std::string("::OnLoad");

   //void hkBootUpdate_Awake() {
   //   reinterpret_cast<void(__fastcall*)()>(Memory::baseAddress + 0x572300)(); // BootUpdate::ForceSystemLanguage
   //   CreateThread(NULL, 0, &Internals::hkGameThread, NULL, 0, 0);
   //   // Remove BootUpdate::Awake hook
   //   Memory::writeJMP(0x57189A, false, 0x572300, false);
   //}

   template <typename FuncType>
   void callModFunction(HMODULE& modHandle, const std::string& funcName) {
      if (auto func = GetProcAddress(modHandle, funcName.c_str()))
         std::thread(reinterpret_cast<FuncType>(func)).detach();
   }
   template <typename FuncType, typename ArgType>
   void callModFunction(HMODULE& modHandle, const std::string& funcName, ArgType arg) {
      if (auto func = GetProcAddress(modHandle, funcName.c_str()))
         std::thread(reinterpret_cast<FuncType>(func), arg).detach();
   }
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
      auto& modsDir          = gameDir / strModsFolderName;

      // Load MirrorHook
      if (!std::filesystem::exists(gameDir / L"MirrorHook.dll")) {
         MessageBoxW(NULL, L"MirrorHook.dll is missing. Some mods may not work/game might be unstable.", L"TLD ModLoader - WARNING", MB_ICONWARNING);
      } else {
         if (!LoadLibraryW((gameDir / L"MirrorHook.dll").c_str())) {
            MessageBoxW(NULL, L"MirrorHook could not be loaded. Some mods may not work and game might be unstable.", L"TLD ModLoader - WARNING", MB_ICONWARNING);
         } else {
            loadedMirrorHook = true;
         }
      }

      // Load mods
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

      // Call Mod::Pre
      for (auto& modDLLHandle : loadedMods)
         callModFunction<Internals::fnNoParam>(modDLLHandle, strFnModPre);

      // Wait for Unity to init
      while (!GetModuleHandleW(L"GameAssembly.dll"))
         Sleep(250);

      // Hook GameManager::Update
      Memory::Init();
      //Memory::writeJMP(0x57189A, false, (DWORD64)&hkBootUpdate_Awake, true);
      Memory::writeCall((DWORD64)&Internals::hkGameManager_Update_ASMProxy, true, 0x7B41B0, false); // Panel_MissionsStory$$MaybeShowNotificationsInQueue
      Memory::writeCall((DWORD64)&Internals::hkGameManager_Update_ASMProxy + 5, true, (DWORD64)&Internals::hkGameManager_Update, true);
      Memory::writeJMP((DWORD64)&Internals::hkGameManager_Update_ASMProxy + 10, true, 0x36FE3B, false);
      Memory::writeJMP(0x36FE36, false, (DWORD64)&Internals::hkGameManager_Update_ASMProxy, true); // InterfaceManager.m_Panel_MissionsStory.MaybeShowNotificationsInQueue();

      // Init MirrorHook
      if (loadedMirrorHook) {
         HWND windowHandle = FindWindowW(NULL, L"TheLongDark");
         while (!windowHandle) {
            windowHandle = FindWindowW(NULL, L"TheLongDark");
            Sleep(1000);
         }

         MirrorHook::PrepareFor(MirrorHook::Framework::UniversalD3D11, &windowHandle);
      }

      // Call Mod::OnLoad
      for (auto& modDLLHandle : loadedMods) {
         // TODO: require config.json from modders, do priority listing
         callModFunction<Internals::fnOneBool>(modDLLHandle, strFnModOnload, loadedMirrorHook);
      }

      return FALSE;
   }
} MODLOADER_NAMESPACE_END()

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH) {
      ModLoader::hThisModule = hModule;
      DisableThreadLibraryCalls(hModule);
      CreateThread(NULL, 0, &ModLoader::Init, NULL, 0, 0);
   }
   return TRUE;
}
