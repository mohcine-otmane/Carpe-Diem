#define MyAppName "Carpe Diem"
#define MyAppVersion "1.0"
#define MyAppPublisher "MOHCINE OTMANE, SIDI BENNOUR, BRAGA, MOROCCO"
#define MyAppExeName "countdown.exe"

[Setup]
AppId={{8F7B3A9D-4B5C-4E8F-9F2D-1A3B4C5D6E7F}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir=installer
OutputBaseFilename=CarpeDiem-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=icon.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "dist\countdown.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\SDL2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\SDL2_ttf.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\SDL2_image.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Technology.ttf"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\config.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\quotes.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "icon.png"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent 