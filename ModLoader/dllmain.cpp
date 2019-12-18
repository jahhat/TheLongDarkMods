#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <string>

HMODULE hThisModule = NULL;
const std::string szModsFolderName = "mods";

DWORD WINAPI Init(LPVOID) {
   wchar_t gameDir[MAX_PATH] ={ 0 };
   // Get TLD's directory, for some reason 'std::filesystem::current_path()' returns the root directory
   {
      if (!GetModuleFileNameW(hThisModule, gameDir, _countof(gameDir))) {
         std::wstring msg = L"GetModuleFileNameW failed.\n\nError:\n" + std::to_wstring(GetLastError());
         MessageBox(NULL, msg.c_str(), L"TLD ModLoader", MB_ICONERROR);

         return FALSE;
      }
   }

   auto& modsDir = std::filesystem::path(gameDir).parent_path() / szModsFolderName;
   if (std::filesystem::exists(modsDir) && std::filesystem::is_directory(modsDir)) {
      SetCurrentDirectory(modsDir.c_str());

      for (auto& file : std::filesystem::recursive_directory_iterator(modsDir)) {
         if (!std::filesystem::is_regular_file(file)) // is not a link, directory or pipe
            continue;
         if (file.path().extension().string() != ".dll") // is a dll file
            continue;
         if (GetModuleHandle(file.path().c_str())) // is not already loaded
            continue;

         auto& modAbsolutePath = modsDir / file.path().filename();
         auto  handle = LoadLibrary(modAbsolutePath.c_str());
         SetCurrentDirectory(modsDir.c_str()); // mod may change this

         if (!handle) {
            auto e = GetLastError();
            if (e != ERROR_DLL_INIT_FAILED) {
               std::wstring msg = L"Unable to load " + modAbsolutePath.wstring() + L".\n\nError:\n" + std::to_wstring(e);
               MessageBox(NULL, msg.c_str(), L"TLD ModLoader", MB_ICONERROR);
            }
         }
      }
   }

   return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
   if (reason == DLL_PROCESS_ATTACH) {
      hThisModule = hModule;
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, 0);
   }
   return TRUE;
}

