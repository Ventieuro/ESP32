# flash.ps1 - compila, carica e apre il monitor seriale per uno sketch ESP32
#
# Uso:
#   .\tools\flash.ps1                      # sketch "blink_test", porta auto
#   .\tools\flash.ps1 -Sketch blink_test   # sketch specifico
#   .\tools\flash.ps1 -Port COM3           # porta specifica
#   .\tools\flash.ps1 -NoMonitor           # non aprire il monitor seriale dopo l'upload
#
# Premi CTRL+C per chiudere il monitor seriale.

param(
    [string]$Sketch = "blink_test",
    [string]$Port = "",
    [string]$Fqbn = "esp32:esp32:esp32",
    [int]$Baud = 115200,
    [switch]$NoMonitor
)

$ErrorActionPreference = "Stop"

# arduino-cli incluso nell'Arduino IDE (winget). Modifica il percorso se l'IDE e' altrove.
$cli = "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\ArduinoSA.IDE.stable_Microsoft.Winget.Source_8wekyb3d8bbwe\resources\app\lib\backend\resources\arduino-cli.exe"
if (-not (Test-Path $cli)) {
    $cli = (Get-Command arduino-cli -ErrorAction SilentlyContinue).Source
}
if (-not $cli) { throw "arduino-cli non trovato. Installa Arduino IDE o arduino-cli." }

$repo = Split-Path $PSScriptRoot -Parent
$sketchPath = Join-Path $repo $Sketch
if (-not (Test-Path $sketchPath)) { throw "Sketch non trovato: $sketchPath" }

# Auto-rilevamento porta: prende la prima porta seriale USB
if (-not $Port) {
    $json = & $cli board list --format json | ConvertFrom-Json
    $Port = ($json.detected_ports | Where-Object { $_.port.protocol -eq "serial" -and $_.port.properties.vid } |
             Select-Object -First 1).port.address
    if (-not $Port) { throw "Nessuna porta seriale USB rilevata. Collega l'ESP32 o passa -Port COMx." }
    Write-Host "Porta rilevata: $Port" -ForegroundColor Cyan
}

Write-Host "==> Compilo $Sketch ..." -ForegroundColor Yellow
& $cli compile --fqbn $Fqbn $sketchPath
if ($LASTEXITCODE -ne 0) { throw "Compilazione fallita." }

Write-Host "==> Carico su $Port ..." -ForegroundColor Yellow
& $cli upload -p $Port --fqbn $Fqbn $sketchPath
if ($LASTEXITCODE -ne 0) { throw "Upload fallito. Se resta su 'Connecting...', tieni premuto BOOT sulla scheda." }

Write-Host "==> Fatto." -ForegroundColor Green

if (-not $NoMonitor) {
    Write-Host "==> Monitor seriale ($Baud baud) - CTRL+C per uscire" -ForegroundColor Cyan
    & $cli monitor -p $Port --config baudrate=$Baud
}
