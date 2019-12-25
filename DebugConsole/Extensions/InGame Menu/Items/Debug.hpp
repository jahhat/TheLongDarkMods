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
         bool isFlyModeOn = false;

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
            if (ImGui::IsKeyPressed(VK_F1, false)) {
               isFlyModeOn = !isFlyModeOn;
               if (isFlyModeOn)
                  Mod::ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(Mod::GetBaseAddress() + 0x5C17B0)(); });
               else
                  Mod::ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(Mod::GetBaseAddress() + 0x5C1D20)(); });
            }
            ImGui::PushItemDisabled();
            ImGui::Checkbox("Fly Mode [F1]", &isFlyModeOn);
            ImGui::PopItemDisabled();
            return true;
         }
      };
   }
}