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

using System;
using System.Windows;
using System.IO;
using Microsoft.WindowsAPICodePack.Dialogs;

namespace ModLoaderInstaller {
   public partial class MainWindow : Window {
      public MainWindow() {
         InitializeComponent();
      }

      private void BtnBrowseGameDir_Click(Object sender, RoutedEventArgs e) {
         using (var dialog = new CommonOpenFileDialog() {
            IsFolderPicker = true
         }) {
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok) {
               if (!File.Exists(Path.Combine(dialog.FileName, "TLD.exe"))) {
                  MessageBox.Show("Selected folder does not have 'TLD.exe'! Please select a valid TLD folder.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);

               } else if (!File.Exists(Path.Combine(dialog.FileName, "GameAssembly.dll"))) {
                  MessageBox.Show("Selected folder does not have 'GameAssembly.dll'! Please select a valid TLD v1.60+ folder.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
               } else {
                  txtBoxGameDir.Text = dialog.FileName;
                  btnInstallModLoader.IsEnabled = true;
               }
            }
         }
      }
      private void BtnInstallModLoader_Click(Object sender, RoutedEventArgs e) {
         try {
            if (!File.Exists("ModLoader.dll") || !File.Exists("ModLoaderProxy.dll") || !File.Exists("MirrorHook.dll")) {
               MessageBox.Show("Mod loader installation files are missing. Are you running this from the archive?\r\n" +
                  "If so, extract all and restart the installation.\r\n" +
                  "If not, archive may be corrupt. Redownload the mod loader.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
               return;
            }

            var gameDir = txtBoxGameDir.Text;
            // Rename original UnityPlayer
            {
               var origUnityPlayer = Path.Combine(gameDir, "UnityPlayer.dll");
               var newUnityPlayer = Path.Combine(gameDir, "RealUnityPlayer.dll");

               if (File.Exists(newUnityPlayer)) {
                  var dialogResult = MessageBox.Show("Are you reinstalling the mod loader after a game update?", "TLD Mod Loader Installer", MessageBoxButton.YesNo, MessageBoxImage.Question);
                  if (dialogResult == MessageBoxResult.Yes) {
                     File.Copy(origUnityPlayer, newUnityPlayer, true);
                     File.Delete(origUnityPlayer);
                  }
               } else {
                  File.Copy(origUnityPlayer, newUnityPlayer, true);
                  File.Delete(origUnityPlayer);
               }

            }
            // Install ModLoader
            {
               File.Copy("MirrorHook.dll", Path.Combine(gameDir, "MirrorHook.dll"), true);
               File.Copy("ModLoader.dll", Path.Combine(gameDir, "ModLoader.dll"), true);
               File.Copy("ModLoaderProxy.dll", Path.Combine(gameDir, "UnityPlayer.dll"), true);

               if (!Directory.Exists(Path.Combine(gameDir, "Mods")))
                  Directory.CreateDirectory(Path.Combine(gameDir, "Mods"));
            }
            // Check for "Mods" folder near the installer file
            {
               if (Directory.Exists("Mods") && Directory.GetFiles("Mods").Length > 0) {
                  var dialogResult = MessageBox.Show("Would you like to install the mods inside the 'Mods' folder?", "TLD Mod Loader Installer", MessageBoxButton.YesNo, MessageBoxImage.Question);
                  if (dialogResult == MessageBoxResult.Yes) {
                     foreach (var file in Directory.EnumerateFiles("Mods")) {
                        if (file.EndsWith(".dll"))
                           File.Copy(file, Path.Combine(gameDir, file), true);
                     }
                  }
               }
            }

            lblInstallStatus.Visibility = Visibility.Visible;
         } catch (DirectoryNotFoundException) {
            MessageBox.Show("Selected TLD folder no longer exists. Installation cancelled.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
         } catch (ArgumentException) {
            MessageBox.Show("Selected TLD folder is missing 'UnityPlayer.dll'. Installation cancelled.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
         } catch (FileNotFoundException ex) {
            if (ex.FileName == "ModLoader.dll" || ex.FileName == "ModLoaderProxy.dll" || ex.FileName == "MirrorHook.dll")
               MessageBox.Show("Mod loader installation files are missing. Installation cancelled.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
         } catch (IOException) {
            MessageBox.Show("TLD is currently running. Please close the game and restart the installation.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
         } catch (UnauthorizedAccessException) {
            MessageBox.Show("Installer cannot access the game files. Installation cancelled.\r\nTry running the installer as administrator.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
         }
      }
   }
}
