#!/usr/bin/env bash
# flash.sh - compila, carica e apre il monitor seriale per uno sketch ESP32 (macOS/Linux)
#
# Uso:
#   ./tools/flash.sh                      # sketch "blink_test", porta auto
#   ./tools/flash.sh --sketch oled_test   # sketch specifico
#   ./tools/flash.sh --port /dev/cu.usbserial-0001   # porta specifica
#   ./tools/flash.sh --no-monitor         # non aprire il monitor seriale dopo l'upload
#
# Premi CTRL+C per chiudere il monitor seriale.

set -euo pipefail

SKETCH="blink_test"
PORT=""
FQBN="esp32:esp32:esp32"
BAUD="115200"
NO_MONITOR=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --sketch) SKETCH="$2"; shift 2 ;;
    --port) PORT="$2"; shift 2 ;;
    --fqbn) FQBN="$2"; shift 2 ;;
    --baud) BAUD="$2"; shift 2 ;;
    --no-monitor) NO_MONITOR=1; shift ;;
    *) echo "Opzione sconosciuta: $1" >&2; exit 1 ;;
  esac
done

# arduino-cli incluso nell'Arduino IDE, oppure installato a parte nel PATH.
CLI=""
for candidate in \
  "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" \
  "$(command -v arduino-cli || true)"; do
  if [[ -n "$candidate" && -x "$candidate" ]]; then
    CLI="$candidate"
    break
  fi
done
if [[ -z "$CLI" ]]; then
  echo "arduino-cli non trovato. Installa Arduino IDE o arduino-cli." >&2
  exit 1
fi

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SKETCH_PATH="$REPO/$SKETCH"
if [[ ! -d "$SKETCH_PATH" ]]; then
  echo "Sketch non trovato: $SKETCH_PATH" >&2
  exit 1
fi

# Auto-rilevamento porta: prende la prima porta seriale USB rilevata da arduino-cli.
if [[ -z "$PORT" ]]; then
  PORT="$("$CLI" board list --format json | python3 -c '
import json, sys
data = json.load(sys.stdin)
for p in data.get("detected_ports", []):
    port = p.get("port", {})
    if port.get("protocol") == "serial" and port.get("properties", {}).get("vid"):
        print(port.get("address", ""))
        break
')"
  if [[ -z "$PORT" ]]; then
    echo "Nessuna porta seriale USB rilevata. Collega l'ESP32 o passa --port /dev/cu.xxx." >&2
    exit 1
  fi
  echo "Porta rilevata: $PORT"
fi

echo "==> Compilo $SKETCH ..."
"$CLI" compile --fqbn "$FQBN" "$SKETCH_PATH"

echo "==> Carico su $PORT ..."
if ! "$CLI" upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_PATH"; then
  echo "Upload fallito. Se resta su 'Connecting...', tieni premuto BOOT sulla scheda." >&2
  exit 1
fi

echo "==> Fatto."

if [[ "$NO_MONITOR" -eq 0 ]]; then
  echo "==> Monitor seriale ($BAUD baud) - CTRL+C per uscire"
  "$CLI" monitor -p "$PORT" --config "baudrate=$BAUD"
fi
