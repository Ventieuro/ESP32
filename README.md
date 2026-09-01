# ESP32

Progetto di test per scheda **ESP32 DevKit V1 (ESP32-WROOM-32)**.

## Contenuto

| Percorso | Descrizione |
|---|---|
| `blink_test/blink_test.ino` | Sketch di verifica: fa lampeggiare il LED integrato e stampa sul monitor seriale |
| `tools/flash.ps1` | Script: compila + carica + apre il monitor seriale (auto-rileva la porta) |
| [`SETUP.md`](SETUP.md) | Guida completa: installazione, driver CP2102, problemi comuni e soluzioni |

## Modificare il codice e ricaricarlo

**Con Arduino IDE:** apri lo `.ino`, modifica, premi **Upload** (→). Ogni upload sovrascrive il
programma precedente sulla scheda.

**Con lo script (da PowerShell, nella cartella del repo):**
```powershell
.\tools\flash.ps1
```
Compila `blink_test`, lo carica sulla scheda e apre il monitor seriale. Opzioni:
`-Sketch <nome>`, `-Port COM3`, `-NoMonitor`. CTRL+C chiude il monitor.

## Come testare la scheda (Windows + Arduino IDE)

### 1. Driver USB-seriale
Collega l'ESP32 con un cavo USB **dati**. In *Gestione dispositivi → Porte (COM e LPT)* deve comparire:
- `Silicon Labs CP210x` (driver CP210x di Silicon Labs), oppure
- `USB-SERIAL CH340` (driver CH340).

Annota il numero di porta (es. `COM3`).

### 2. Supporto ESP32 in Arduino IDE
1. *File → Preferenze → URL aggiuntive per il Gestore schede*:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
2. *Strumenti → Scheda → Gestore schede* → cerca `esp32` → installa **esp32 by Espressif Systems**.

### 3. Selezione
- *Strumenti → Scheda →* **ESP32 Dev Board**
  (nel pacchetto ESP32 3.x si chiama così; nelle guide più vecchie è *"ESP32 Dev Module"* — è la stessa scheda)
- *Strumenti → Porta →* la COM annotata sopra

### 4. Upload
Apri `blink_test/blink_test.ino` e premi **Upload**.
Se resta bloccato su `Connecting....____`, tieni premuto il pulsante **BOOT** finché parte la scrittura.

### 5. Verifica
- Il LED blu sulla scheda lampeggia ogni 0,5 s.
- *Strumenti → Monitor seriale* a **115200 baud**: scorre `LED ON` / `LED OFF`.
