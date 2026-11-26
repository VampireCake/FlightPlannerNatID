param(
    [string]$NatIdSdkRoot = "$env:USERPROFILE\natID.SDK"
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$buildDirectory = Join-Path $repoRoot 'build'
$stageDirectory = Join-Path $buildDirectory 'stage'
$dependencyDirectory = Join-Path $buildDirectory 'dependencies'
$vcRedistPath = Join-Path $dependencyDirectory 'vc_redist.x64.exe'
$vcRedistUrl = 'https://aka.ms/vc14/vc_redist.x64.exe'
$installerScript = Join-Path $PSScriptRoot 'FlightPlannerNatID.iss'

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw 'CMake 3.21 or newer is required. Install CMake and retry.'
}

if (-not (Test-Path (Join-Path $NatIdSdkRoot 'DevEnv\Common.cmake'))) {
    throw "natID SDK was not found at '$NatIdSdkRoot'. Pass its folder with -NatIdSdkRoot."
}

$isccCandidates = @(
    (Join-Path $buildDirectory 'tools\InnoSetup\ISCC.exe'),
    (Join-Path $env:ProgramFiles 'Inno Setup 7\ISCC.exe'),
    (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6\ISCC.exe'),
    (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 7\ISCC.exe'),
    (Join-Path $env:ProgramFiles 'Inno Setup 6\ISCC.exe')
)
$iscc = $isccCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
if (-not $iscc) {
    throw 'Inno Setup 6 or 7 is required. Install it from https://jrsoftware.org/isinfo.php and retry.'
}

New-Item -ItemType Directory -Path $dependencyDirectory -Force | Out-Null
if (-not (Test-Path -LiteralPath $vcRedistPath) -or
    (Get-Item -LiteralPath $vcRedistPath).Length -lt 1MB) {
    Invoke-WebRequest -Uri $vcRedistUrl -OutFile $vcRedistPath
}

$signature = Get-AuthenticodeSignature -FilePath $vcRedistPath
if ($signature.Status -ne 'Valid' -or
    $signature.SignerCertificate.Subject -notmatch 'Microsoft Corporation') {
    throw 'The downloaded Visual C++ runtime did not pass Microsoft signature verification.'
}

Push-Location $repoRoot
try {
    & cmake -S . -B build "-DNATID_SDK_ROOT=$NatIdSdkRoot" -DFLIGHTPLANNER_SKIP_DEVRES_REGISTRATION=ON
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }

    & cmake --build build --config Release
    if ($LASTEXITCODE -ne 0) { throw 'Release build failed.' }

    if (Test-Path -LiteralPath $stageDirectory) {
        $expectedStage = [IO.Path]::GetFullPath($stageDirectory)
        $actualStage = [IO.Path]::GetFullPath((Resolve-Path -LiteralPath $stageDirectory).Path)
        if ($actualStage -ne $expectedStage -or
            -not $actualStage.StartsWith([IO.Path]::GetFullPath($buildDirectory), [StringComparison]::OrdinalIgnoreCase)) {
            throw 'Refusing to remove an unexpected staging directory.'
        }
        Remove-Item -LiteralPath $stageDirectory -Recurse -Force
    }

    & cmake --install build --config Release --prefix $stageDirectory
    if ($LASTEXITCODE -ne 0) { throw 'Could not stage the application runtime files.' }

    Copy-Item -LiteralPath $vcRedistPath -Destination $stageDirectory -Force
    New-Item -ItemType Directory -Path (Join-Path $repoRoot 'installer-output') -Force | Out-Null

    & $iscc $installerScript
    if ($LASTEXITCODE -ne 0) { throw 'Inno Setup could not create the installer.' }
}
finally {
    Pop-Location
}

$output = Join-Path $repoRoot 'installer-output\FlightPlanner-Setup-0.1.0.exe'
Write-Output "Installer created: $output"
