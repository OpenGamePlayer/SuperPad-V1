# SuperPad-V1 / Alpakka - Arduino build script (Windows)
#
# Usage:  powershell -ExecutionPolicy Bypass -File build.ps1
#    or:  ./build.ps1
#
# Prereqs: arduino-cli on PATH (https://arduino.github.io/arduino-cli/)
#          earlephilhower rp2040 core:
#            arduino-cli core install rp2040:rp2040
#
# Output: %LOCALAPPDATA%\arduino\sketches\<hash>\RP2040.ino.uf2

$ErrorActionPreference = "Stop"

# 1) arduino-cli (adjust if not on PATH)
$cli = "arduino-cli"
if (Test-Path "D:\Projects\OGP\_tools\arduino-cli\arduino-cli.exe") {
    $cli = "D:\Projects\OGP\_tools\arduino-cli\arduino-cli.exe"
}

$dir = $PSScriptRoot
if ($PSScriptRoot -notmatch "RP2040$") { $dir = Join-Path $PSScriptRoot "RP2040" }

# 2) locate arduino-pico core
$core = Get-ChildItem "$env:LOCALAPPDATA\Arduino15\packages\rp2040\hardware\rp2040" -Directory `
    -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
if (-not $core) { throw "rp2040 core not found. Run: arduino-cli core install rp2040:rp2040" }
Write-Host "Core: $($core.FullName)"

# 3) extra include paths
$incHeaders = Join-Path $dir "src\headers"
$incSrc     = Join-Path $dir "src"
# hardware_rosc headers are not in core_inc.txt; add manually:
$incRosc    = Join-Path $core.FullName "pico-sdk\src\rp2_common\hardware_rosc\include"

# 4) compile flags
$flags = "-DDEVICE_ALPAKKA_V0=1 -DDEVICE_IS_ALPAKKA=1 " +
         "-I `"$incSrc`" -I `"$incHeaders`" -I `"$incRosc`" -Wno-error"

# 5) compile (Adafruit TinyUSB stack provides HID/VENDOR classes)
& $cli compile --fqbn "rp2040:rp2040:rpipico:usbstack=tinyusb" `
    --build-property "compiler.cpp.extra_flags=$flags" `
    --build-property "compiler.c.extra_flags=$flags" `
    $dir

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "BUILD OK"
    $uf2 = Get-ChildItem "$env:LOCALAPPDATA\arduino\sketches" -Recurse -Filter "RP2040.ino.uf2" `
        -ErrorAction SilentlyContinue | Select-Object -Last 1
    if ($uf2) { Write-Host "UF2: $($uf2.FullName)" }
} else {
    Write-Host "BUILD FAILED" -ForegroundColor Red
}
exit $LASTEXITCODE