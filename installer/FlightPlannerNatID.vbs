Option Explicit

Dim shell, fileSystem, appDirectory, processEnvironment, commandLine
Set shell = CreateObject("WScript.Shell")
Set fileSystem = CreateObject("Scripting.FileSystemObject")

appDirectory = fileSystem.GetParentFolderName(WScript.ScriptFullName)
shell.CurrentDirectory = appDirectory

Set processEnvironment = shell.Environment("PROCESS")
processEnvironment("PATH") = appDirectory & ";" & _
    appDirectory & "\GTK;" & _
    appDirectory & "\GTK\release\lib;" & _
    appDirectory & "\GTK\release\lib\gtk-4.0;" & _
    processEnvironment("PATH")
processEnvironment("GTK_EXE_PREFIX") = appDirectory & "\GTK"
processEnvironment("GTK_DATA_PREFIX") = appDirectory & "\GTK\release"
processEnvironment("GTK_PATH") = appDirectory & "\GTK\release"
processEnvironment("GSETTINGS_SCHEMA_DIR") = _
    appDirectory & "\GTK\release\share\glib-2.0\schemas"
processEnvironment("FONTCONFIG_PATH") = _
    appDirectory & "\GTK\release\share\fontconfig"

commandLine = Chr(34) & appDirectory & "\appFlightPlanner.exe" & Chr(34) & _
    " -devResPath=" & Chr(34) & appDirectory & Chr(34)
shell.Run commandLine, 1, False
