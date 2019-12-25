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
#include "stdafx.h"
#include "_BaseInGameMenuItem.hpp"
#include "Extensions\Extensions.h"

namespace Extensions {
   namespace InGameMenu {
      struct Debug : _BaseInGameMenuItem {
      private:
         bool isFlyModeOn     = false;
         bool isDrawingMenu   = false;
         bool isWaitingScene  = false;

         char inputArray[256] ={ 0 };

         int drawTextboxInCenter(const char* title, ImGuiInputTextFlags flags) {
            static auto& size  = ImVec2(245.0f, 30.0f);
            auto& displaySize  = pImGuiIO->DisplaySize;
            auto& menuPosition = ImVec2((displaySize.x / 2) - (size.x / 2), (displaySize.y / 2) - (size.y / 2));

            ImGui::SetNextWindowSizeConstraints(size, size);
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);
            ImGui::SetNextWindowPos(menuPosition, ImGuiCond_Always);
            if (ImGui::Begin(title ? title : "###InputMenu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
               ImGui::InputText("###InputTextbox", inputArray, _countof(inputArray), flags);
               ImGui::SetKeyboardFocusHere(); ImGui::SameLine();
               ImGui::PushItemDisabled();
               ImGui::Button("[ENTER]");
               ImGui::Button("Press [ESCAPE] to cancel.", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.0f));
               ImGui::PopItemDisabled();
            } ImGui::End();
            if (ImGui::IsKeyPressed(VK_ESCAPE, false))
               return -1;
            else if (ImGui::IsKeyPressed(VK_RETURN, false))
               return 1;

            return 0;
         }

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.98f, 0.59f, 0.26f, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.98f, 0.59f, 0.26f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.98f, 0.53f, 0.06f, 1.00f));
            bool ret = ImGui::Button("Debug [F1]", buttonSize);
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            return ret || ImGui::IsKeyPressed(VK_F1, false);
         }

         const virtual bool displayMenu() override {
            if (!isDrawingMenu) {
               if (ImGui::IsKeyPressed(VK_F1, false)) {
                  isFlyModeOn = !isFlyModeOn;
                  if (isFlyModeOn)
                     Mod::ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(Mod::GetBaseAddress() + 0x5C17B0)(); });
                  else
                     Mod::ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(Mod::GetBaseAddress() + 0x5C1D20)(); });
               } else if (ImGui::IsKeyPressed(VK_F2, false)) {
                  ZeroMemory(inputArray, _countof(inputArray));
                  isDrawingMenu = isWaitingScene = true;
               }
            } else {
               if (isWaitingScene) {
                  auto ret = drawTextboxInCenter("Enter scene index (decimal)", ImGuiInputTextFlags_CharsDecimal);
                  if (ret == 1) {
                     int sceneIndex = atoi(inputArray);
                     Mod::ExecuteInGameThread([sceneIndex]() { reinterpret_cast<void(__fastcall*)(int, int)>(Mod::GetBaseAddress() + 0x98E880)(sceneIndex, 0); });
                     isDrawingMenu = isWaitingScene = false;
                  } else if (ret == -1) {
                     isDrawingMenu = isWaitingScene = false;
                  }
               }
            }

            ImGui::PushItemDisabled();
            ImGui::Checkbox("Fly Mode [F1]", &isFlyModeOn);
            ImGui::Button("Load Scene by index [F2]");
            ImGui::PopItemDisabled();
            return true;
         }
      };
   }
}