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
#include <cstdint>
#include "../UnityTypes/Quaternion.hpp"
#include "../UnityTypes/Vector3.hpp"

MOD_NAMESPACE_BEGIN() {
   struct FlyMode {
      static inline uint64_t _ClassInstanceOffset = 0x3C15A08;

      /* +0x00 */ bool                               m_Enabled;
      /* +0x04 */ UNITY_ENGINE_NAMESPACE::Vector3    m_RestoreCameraPos;
      /* +0x10 */ UNITY_ENGINE_NAMESPACE::Quaternion m_RestoreCameraOrient;
      /* +0x20 */ float                              m_SetTimeOfDay;
      /* +0x28 */ LPVOID                             m_Camera;
      /* +0x30 */ bool                               m_IgnoreDownUntilDownIsUp;

      // Instance vars
      ///* +0x18 */ float m_MoveSpeedSlow; // 0x18
      ///* +0x1C */ float m_MoveSpeedNormal; // 0x1C
      ///* +0x20 */ float m_MoveSpeedFast; // 0x20

      void Enter() {
         ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(GetBaseAddress() + 0x5C17B0)(); });
      }
      void Exit() {
         ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(GetBaseAddress() + 0x5C1AB0)(); });
      }
      void TeleportPlayerAndExit() {
         Mod::ExecuteInGameThread([]() { reinterpret_cast<void(__fastcall*)()>(Mod::GetBaseAddress() + 0x5C1D20)(); });
      }
      void Warp(UNITY_ENGINE_NAMESPACE::Vector3* pPos, UNITY_ENGINE_NAMESPACE::Quaternion* pRot, float setTimeOfDay = -1.0f) {
         Mod::ExecuteInGameThread([pPos, pRot, setTimeOfDay]() {
            reinterpret_cast<void(__fastcall*)(UNITY_ENGINE_NAMESPACE::Vector3*, UNITY_ENGINE_NAMESPACE::Quaternion*, float)>(Mod::GetBaseAddress() + 0x5C2030)
               (pPos, pRot, setTimeOfDay);
                                  }
         );
      }
   };
} MOD_NAMESPACE_END()
