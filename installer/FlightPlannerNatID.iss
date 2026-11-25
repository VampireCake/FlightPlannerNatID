#define AppName "Flight Planner"
#define AppVersion "0.1.0"
#define AppPublisher "Flight Planner"
#define StageDirectory "..\build\stage"

[Setup]
AppId={{3F125A62-0CD9-46FC-92B2-4B39598D84BD}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\Flight Planner
DefaultGroupName={#AppName}
UninstallDisplayIcon={app}\appFlightPlanner.exe
SetupIconFile=..\res\PNG-like\ikonica.ico
OutputDir=..\installer-output
OutputBaseFilename=FlightPlanner-Setup-{#AppVersion}
PrivilegesRequired=admin
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
ChangesAssociations=no

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Files]
Source: "{#StageDirectory}\*"; DestDir: "{app}"; Excludes: "vc_redist.x64.exe"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#StageDirectory}\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Icons]
Name: "{autoprograms}\{#AppName}"; Filename: "{sys}\wscript.exe"; Parameters: """{app}\FlightPlannerNatID.vbs"""; WorkingDir: "{app}"; IconFilename: "{app}\appFlightPlanner.exe"
Name: "{autodesktop}\{#AppName}"; Filename: "{sys}\wscript.exe"; Parameters: """{app}\FlightPlannerNatID.vbs"""; WorkingDir: "{app}"; IconFilename: "{app}\appFlightPlanner.exe"; Tasks: desktopicon

[Run]
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Microsoft Visual C++ Runtime..."; Flags: runhidden waituntilterminated
Filename: "{sys}\wscript.exe"; Parameters: """{app}\FlightPlannerNatID.vbs"""; WorkingDir: "{app}"; Description: "Launch {#AppName}"; Flags: postinstall nowait skipifsilent
