#define DOSBoxAppName "DOSBox Staging"
#define DOSBoxAppDirName "dosbox-staging"
#define DOSBoxAppVersion "0.78.1"
#define DOSBoxAppURL "https://dosbox-staging.github.io/"
#define DOSBoxAppExeName "dosbox.exe"
#define DOSBoxAppBuildDate GetDateTimeString('yyyymmdd_hhnnss', '', '')

[Setup]
AppId={{587471B7-02F6-4F25-8D00-70006790653C}
AppName={#DOSBoxAppName}
AppVersion={#DOSBoxAppVersion}
AppVerName={#DOSBoxAppName} {#DOSBoxAppVersion}
AppPublisherURL={#DOSBoxAppURL}
AppSupportURL={#DOSBoxAppURL}
AppUpdatesURL={#DOSBoxAppURL}
DefaultDirName={sd}\{#DOSBoxAppDirName}
DisableDirPage=no
DisableWelcomePage=no
DisableProgramGroupPage=yes
InfoBeforeFile=setup_preamble.txt
OutputDir=.\
OutputBaseFilename=dosbox-staging-{#DOSBoxAppVersion}-setup
SetupIconFile=dosbox-staging.ico
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest
;PrivilegesRequiredOverridesAllowed=dialog
UninstallDisplayIcon={app}\{#DOSBoxAppExeName}
WizardSmallImageFile=dosbox-staging.bmp

[Messages]
InfoBeforeLabel=Please read the general information about DOSBox Staging below.

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "contextmenu"; Description: "Add ""Run/Open with DOSBox Staging"" context menu for Windows Explorer"
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Run]
Filename: "{app}\{#DOSBoxAppExeName}"; Parameters: "-c ""config -wc"""; Description: "{cm:LaunchProgram,{#StringChange(DOSBoxAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Files]
Source: "program\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Registry]
Root: HKA; Subkey: "Software\dosbox-staging"; Flags: uninsdeletekeyifempty
Root: HKA; Subkey: "Software\dosbox-staging"; ValueType: string; ValueName: "Path"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\dosbox-staging"; ValueType: string; ValueName: "Version"; ValueData: "{#DOSBoxAppVersion}"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\dosbox-staging"; ValueType: string; ValueName: ""; ValueData: "Open with DOSBox Staging"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\Directory\shell\dosbox-staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\dosbox-staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" ""%v"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\dosbox-staging"; ValueType: string; ValueName: ""; ValueData: "Open with DOSBox Staging"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\dosbox-staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\dosbox-staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" ""%v"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.exe\shell\Run with DOSBox Staging"; ValueType: none; ValueName: ""; ValueData: ""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.exe\shell\Run with DOSBox Staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.exe\shell\Run with DOSBox Staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" ""%1"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.com\shell\Run with DOSBox Staging"; ValueType: none; ValueName: ""; ValueData: ""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.com\shell\Run with DOSBox Staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.com\shell\Run with DOSBox Staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" ""%1"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.bat\shell\Run with DOSBox Staging"; ValueType: none; ValueName: ""; ValueData: ""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.bat\shell\Run with DOSBox Staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.bat\shell\Run with DOSBox Staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" ""%1"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.conf\shell\Open with DOSBox Staging"; ValueType: none; ValueName: ""; ValueData: ""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.conf\shell\Open with DOSBox Staging"; ValueType: string; ValueName: "Icon"; ValueData: """{app}\dosbox.exe"",0"; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.conf\shell\Open with DOSBox Staging\command"; ValueType: string; ValueName: ""; ValueData: """{app}\dosbox.exe"" -conf ""%1"""; Check: WizardIsTaskSelected('contextmenu'); Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\dosbox-staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\Directory\shell\dosbox-staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\dosbox-staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\Directory\Background\shell\dosbox-staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.exe\shell\Run with DOSBox Staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.exe\shell\Run with DOSBox Staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.com\shell\Run with DOSBox Staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.com\shell\Run with DOSBox Staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.bat\shell\Run with DOSBox Staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.bat\shell\Run with DOSBox Staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.conf\shell\Open with DOSBox Staging"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.conf\shell\Open with DOSBox Staging\command"; ValueType: none; Check: not WizardIsTaskSelected('contextmenu'); Flags: deletekey

[UninstallDelete]
Type: files; Name: "{app}\stderr.txt"

[Code]
procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID = wpFinished then
    WizardForm.RunList.Visible := False;
end;

