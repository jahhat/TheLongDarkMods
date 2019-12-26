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
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Net;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

using Microsoft.WindowsAPICodePack.Dialogs;

using Newtonsoft.Json.Linq;

namespace ModLoaderInstaller {
   public class CustomTraceListener : TraceListener {
      public override void Fail(String message, String detailedMessage) {
         MessageBox.Show(detailedMessage, message, MessageBoxButton.OK, MessageBoxImage.Error);
      }

      public override void Write(String message) {
         this.WriteLine(message);
      }

      public override void WriteLine(String message) {
         Console.WriteLine(message);
      }
   }
   public class CustomMultiValueConverter : IMultiValueConverter {
      public object Convert(Object[] values, Type targetType, Object parameter, System.Globalization.CultureInfo culture) {
         Boolean a = true;
         foreach (Object value in values) {
            if (value is bool b) {
               a &= b;
            }
         }
         return a;
      }
      public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture) {
         throw new NotSupportedException();
      }
   }

   public class DownloadManager : INotifyPropertyChanged {
      public enum DownloadState {
         NotConnected = 0,
         NoInternetConnection,
         FilesNeedUpdate,
         WaitingForGitHubAPI,
         ErrorWhileConnecting,
         ParsingGitHubAPIResponse,
         Downloading,
         ErrorWhileDownloading,
         Extracting,
         ErrorWhileExtracting,
         Ready
      };

      public static readonly String uriLatestReleaseAPI = "https://api.github.com/repos/berkayylmao/TheLongDarkMods/releases/latest";
      private String jsonFromAPI;

      public event PropertyChangedEventHandler PropertyChanged;
      private void onPropertyChanged(String propertyName) {
         PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
      }

      private Boolean readyToInstall = false;
      public Boolean ReadyToInstall {
         get {
            return readyToInstall;
         }
         set {
            if (readyToInstall != value) {
               readyToInstall = value;
               onPropertyChanged("ReadyToInstall");
            }
         }
      }

      private DownloadState curState = DownloadState.NotConnected;
      public DownloadState CurState {
         get {
            return curState;
         }
         set {
            if (curState != value) {
               curState = value;
               onPropertyChanged("CurStateString");

               if (value == DownloadState.Ready)
                  ReadyToInstall = true;
            }
         }
      }
      public String CurStateString {
         get {
            switch (curState) {
               case DownloadState.NotConnected:
                  return "Connecting...";
               case DownloadState.NoInternetConnection:
                  return "No internet connection!";
               case DownloadState.FilesNeedUpdate:
                  return "Local files need to be updated.";
               case DownloadState.WaitingForGitHubAPI:
                  return "Connecting to the GitHub repo...";
               case DownloadState.ErrorWhileConnecting:
                  return "There was an error while connecting to the GitHub repo!";
               case DownloadState.ParsingGitHubAPIResponse:
                  return "Parsing response from GitHub API...";
               case DownloadState.Downloading:
                  return "Downloading new files...";
               case DownloadState.ErrorWhileDownloading:
                  return "There was an error while dowloading new files!";
               case DownloadState.Extracting:
                  return "Extracting new files...";
               case DownloadState.ErrorWhileExtracting:
                  return "There was an error while extracting new files!";
               case DownloadState.Ready:
                  return "Files up-to-date!";
            }

            return String.Empty;
         }
      }

      public Boolean systemHasInternetConnection() {
         // Sorry users from China
         try {
            using (var wc = new WebClient())
            using (wc.OpenRead("https://google.com/"))
               return true;
         } catch (Exception) {
            return false;
         }
      }

      public Boolean filesNeedUpdate() {
         if (!File.Exists("ModLoader\\ModLoader.dll") || !File.Exists("ModLoader\\ModLoaderProxy.dll") || !File.Exists("ModLoader\\MirrorHook.dll"))
            return true;

         using (var wc = new WebClient()) {
            wc.Headers.Add("User-Agent", "berkayylmao:ModLoaderInstaller");
            CurState = DownloadState.WaitingForGitHubAPI;
            try {
               jsonFromAPI = wc.DownloadString(uriLatestReleaseAPI);
               CurState = DownloadState.ParsingGitHubAPIResponse;

               dynamic json = JObject.Parse(jsonFromAPI);
               if ((String)json.tag_name != FileVersionInfo.GetVersionInfo("ModLoader\\ModLoader.dll").FileVersion)
                  return true;
            } catch (WebException) {
               CurState = DownloadState.ErrorWhileConnecting;
            }
         }
         return false;
      }

      public void _thread() {
         if (!systemHasInternetConnection()) {
            CurState = DownloadState.NoInternetConnection;
            return;
         }

         CurState = filesNeedUpdate() ? DownloadState.FilesNeedUpdate : DownloadState.Ready;
         if (CurState == DownloadState.FilesNeedUpdate) {
            using (var wc = new WebClient()) {
               wc.Headers.Add("User-Agent", "berkayylmao:ModLoaderInstaller");
               CurState = DownloadState.WaitingForGitHubAPI;
               try {
                  if (String.IsNullOrWhiteSpace(jsonFromAPI))
                     jsonFromAPI = wc.DownloadString(uriLatestReleaseAPI);

                  CurState = DownloadState.ParsingGitHubAPIResponse;
                  dynamic json = JObject.Parse(jsonFromAPI);
                  try {
                     CurState = DownloadState.Downloading;
                     wc.DownloadFile((String)json.assets[0].browser_download_url, "_Update.zip");
                     try {
                        CurState = DownloadState.Extracting;
                        ZipFile.ExtractToDirectory("_Update.zip", "_Update");
                        foreach (var file in Directory.GetFiles("_Update\\ModLoader", "*.dll", SearchOption.TopDirectoryOnly)) {
                           if (file.EndsWith(".dll")) // Never underestimate Windows and the users
                              File.Copy(file, file.Replace("_Update\\", String.Empty), true);
                        }
                        Directory.Delete("_Update", true);
                        File.Delete("_Update.zip");

                        CurState = DownloadState.Ready;
                     } catch (Exception) {
                        CurState = DownloadState.ErrorWhileExtracting;
                     }
                  } catch (Exception) {
                     CurState = DownloadState.ErrorWhileDownloading;
                  }
               } catch (WebException) {
                  CurState = DownloadState.ErrorWhileConnecting;
               }
            }
         }
      }
   }
   public class GamePathSelectedProxy : INotifyPropertyChanged {
      public event PropertyChangedEventHandler PropertyChanged;

      private Boolean gamePathSelected = false;
      public Boolean GamePathSelected {
         get {
            return gamePathSelected;
         }
         set {
            if (gamePathSelected != value) {
               gamePathSelected = value;
               PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("GamePathSelected"));
            }
         }
      }
   }

   public partial class MainWindow : Window {
      private GamePathSelectedProxy gamePathSelectedProxy = new GamePathSelectedProxy();
      private DownloadManager downloadManager = new DownloadManager();
      private Thread downloaderThread;

      public MainWindow() {
         InitializeComponent();
         if (!Directory.Exists("ModLoader"))
            Directory.CreateDirectory("ModLoader");

         var installModBindings = new MultiBinding() {
            Converter = new CustomMultiValueConverter(),
            NotifyOnSourceUpdated = true
         };
         installModBindings.Bindings.Add(new Binding() {
            Path = new PropertyPath("ReadyToInstall"),
            UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged,
            Mode = BindingMode.OneWay,
            Source = downloadManager
         });
         installModBindings.Bindings.Add(new Binding() {
            Path = new PropertyPath("GamePathSelected"),
            UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged,
            Mode = BindingMode.OneWay,
            Source = gamePathSelectedProxy
         });

         BindingOperations.SetBinding(btnInstallModLoader, Button.IsEnabledProperty, installModBindings);
         BindingOperations.SetBinding(lblStatus, TextBlock.TextProperty, new Binding() {
            Path = new PropertyPath("CurStateString"),
            UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged,
            Mode = BindingMode.OneWay,
            Source = downloadManager
         });

         ServicePointManager.Expect100Continue = true;
         ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12;
         downloaderThread = new Thread(new ThreadStart(downloadManager._thread));
         downloaderThread.Start();

         Trace.Listeners.Clear();
         Trace.Listeners.Add(new CustomTraceListener());
      }

      private void btnBrowseGameDir_Click(Object sender, RoutedEventArgs e) {
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
                  gamePathSelectedProxy.GamePathSelected = true;
               }
            }
         }
      }
      private void btnInstallModLoader_Click(Object sender, RoutedEventArgs e) {
         try {
            Trace.Assert(File.Exists("ModLoader\\ModLoader.dll"), "Error", "ModLoader.dll is missing. ModLoader.dll should be in the 'ModLoader' folder.");
            Trace.Assert(File.Exists("ModLoader\\ModLoaderProxy.dll"), "Error", "ModLoaderProxy.dll is missing. ModLoaderProxy.dll should be in the 'ModLoader' folder.");
            Trace.Assert(File.Exists("ModLoader\\MirrorHook.dll"), "Error", "MirrorHook.dll is missing. MirrorHook.dll should be in the 'ModLoader' folder.");
            if (!File.Exists("ModLoader\\ModLoader.dll") || !File.Exists("ModLoader\\ModLoaderProxy.dll") || !File.Exists("ModLoader\\MirrorHook.dll"))
               return;

            var gameDir = txtBoxGameDir.Text;
            // Rename original UnityPlayer
            {
               var origUnityPlayer = Path.Combine(gameDir, "UnityPlayer.dll");
               var newUnityPlayer = Path.Combine(gameDir, "RealUnityPlayer.dll");

               if (File.Exists(newUnityPlayer)) {
                  var origUnityPlayerFI = FileVersionInfo.GetVersionInfo(origUnityPlayer);
                  if (origUnityPlayerFI.ProductName != "ModLoaderProxy") {
                     File.Copy(origUnityPlayer, newUnityPlayer, true);
                  }
               } else {
                  File.Copy(origUnityPlayer, newUnityPlayer, true);
               }

            }
            // Install ModLoader
            {
               File.Copy("ModLoader\\MirrorHook.dll", Path.Combine(gameDir, "MirrorHook.dll"), true);
               File.Copy("ModLoader\\ModLoader.dll", Path.Combine(gameDir, "ModLoader.dll"), true);
               File.Copy("ModLoader\\ModLoaderProxy.dll", Path.Combine(gameDir, "UnityPlayer.dll"), true);

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
