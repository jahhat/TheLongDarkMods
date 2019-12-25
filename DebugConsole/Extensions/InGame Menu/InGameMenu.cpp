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

#include "stdafx.h"
#include <mutex>
#include "InGameMenu.h"
#include "Extensions/Extensions.h"
// dear imgui
#include "Helpers/imgui/dx11/imgui_impl_dx11.h"
#include "Helpers/imgui/win32/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace Extensions {
   ImGuiIO* pImGuiIO = nullptr;
   namespace InGameMenu {
      _BaseInGameMenuItem* activeItem = nullptr;
      std::vector<_BaseInGameMenuItem*> items ={};

      HWND                  windowHandle        = nullptr;
      ID3D11Device*         pD3DDevice          = nullptr;
      ID3D11DeviceContext*  pD3DDeviceContext   = nullptr;

      bool                  isImguiInitialized  = false;
      bool                  isMainWindowVisible = true;
      static std::once_flag imguiInitLock;

      void hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
         std::call_once(imguiInitLock, [&]() {
            pSwapChain->GetDevice(__uuidof(pD3DDevice), reinterpret_cast<void**>(&pD3DDevice));
            pD3DDevice->GetImmediateContext(&pD3DDeviceContext);

            ImGui::CreateContext();
            pImGuiIO = &ImGui::GetIO();
            ImGui_ImplDX11::Init(pD3DDevice, pD3DDeviceContext);
            ImGui_ImplWin32_Init(windowHandle);

            ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions
                                       | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_PickerHueWheel);
            ImGui::LoadStyle();

            isImguiInitialized = true;
                        }
         );

         ImGui_ImplDX11::NewFrame();
         ImGui_ImplWin32_NewFrame();
         ImGui::NewFrame();

         static float scaling = 1.0f;
         scaling = std::max(1.0f, pImGuiIO->DisplaySize.y / 1080.0f); // optimized for 1080p
         for (int32_t i = 0; i < pImGuiIO->Fonts->Fonts.Size; i++)
            pImGuiIO->Fonts->Fonts[i]->Scale = scaling;

         for (auto item : items) {
            if (item->hasLoadedData) {
               item->onFrame();
            }
         }

         if (isImguiInitialized) {
            if (isMainWindowVisible) {
               ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Once);
               if (ImGui::Begin("###DebugConsole_Main", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
                  static float buttonWidth = 1.0f;
                  // Title
                  ImGui::Text("Debug Console for TLD v1.69");
                  ImGui::Text("by berkayylmao");
                  ImGui::Text("Press Insert to hide/show me.");
                  ImGui::Separator();
                  buttonWidth = ImGui::GetWindowContentRegionWidth();

                  if (!activeItem) {
                     for (auto item : items) {
                        if (item->hasLoadedData && item->displayMenuItem(ImVec2(buttonWidth, 0.0f)))
                           activeItem = item;
                     }
                  } else {
                     if (ImGui::Button("< Back (HOME)"))
                        activeItem = nullptr;
                     else {
                        if (!activeItem->displayMenu())
                           activeItem = nullptr;
                     }
                  }
               }
               ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplDX11::RenderDrawData(ImGui::GetDrawData());
         }
      }

      LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
         if (isImguiInitialized) {
            if (uMsg == WM_QUIT) {
               ImGui_ImplDX11::Shutdown();
               ImGui::DestroyContext();
               return MirrorHook::WndProc::WndProcHook_NoReturn;
            }

            if (isMainWindowVisible)
               ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

            if (!pImGuiIO->WantCaptureKeyboard || !pImGuiIO->WantTextInput) {
               if (uMsg == WM_KEYUP) {
                  switch (wParam) {
                     case VK_HOME:
                        activeItem = nullptr;
                        return FALSE;
                     case VK_INSERT:
                        isMainWindowVisible = !isMainWindowVisible;
                        return FALSE;
                  }
               }
            }
         }
         return MirrorHook::WndProc::WndProcHook_NoReturn;
      }

      void Init() {
         windowHandle = MirrorHook::D3D11::GetWindowHandle();
         MirrorHook::D3D11::AddExtension(MirrorHook::D3D11::D3D11Extension::Present, &hkPresent);
         MirrorHook::WndProc::AddExtension(&wndProcExtension);
         while (!isImguiInitialized) Sleep(1000);
      }
   }
}