#define MyAppName "Countdown Timer"
#define MyAppVersion "1.0"
#define MyAppPublisher "Your Name"
#define MyAppExeName "countdown.exe"

[Setup]
AppId={{YOUR-UNIQUE-APP-ID-HERE}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir=installer
OutputBaseFilename=CountdownTimer-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

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

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent 