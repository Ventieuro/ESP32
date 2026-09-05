# ESP32 Tamagotchi

Un tamagotchi su scheda **ESP32 DevKit V1 (ESP32-WROOM-32)** + display OLED SSD1306 128×64 + 3
bottoni. Nutrilo, giocaci, mettilo a dormire — lo stato sopravvive ai riavvii (salvato in flash).

## Contenuto

| Percorso | Descrizione |
|---|---|
| `tamagotchi/tamagotchi.ino` | Sketch principale: setup, lettura bottoni, ciclo di gioco |
| `tamagotchi/pet.h` / `pet.cpp` | Stato del bug (fame/felicità/energia), logica, salvataggio persistente |
| `tamagotchi/ui.h` / `ui.cpp` | Disegno faccia + barre di stato sull'OLED |
| `tools/flash.ps1` | Script Windows/PowerShell: compila + carica + apre il monitor seriale (auto-rileva la porta) |
| `tools/flash.sh` | Equivalente macOS/Linux di `flash.ps1` |
| [`SETUP.md`](SETUP.md) | Guida completa per **Windows**: installazione, driver CP2102, problemi comuni e soluzioni |
| [`SETUP_MAC.md`](SETUP_MAC.md) | Guida completa per **macOS**: stessa procedura, driver e percorsi porta adattati |

## Bottoni

| Bottone | Pin ESP32 |
|---|---|
| Nutri | GPIO 4 |
| Gioca | GPIO 16 |
| Dormi/Sveglia | GPIO 17 |

Ogni bottone: una gamba a **GND**, l'altra al GPIO (pull-up interno, nessuna resistenza necessaria).

## Modificare il codice e ricaricarlo

**Con Arduino IDE:** apri lo `.ino`, modifica, premi **Upload** (→). Ogni upload sovrascrive il
programma precedente sulla scheda.

**Con lo script (Windows, da PowerShell nella cartella del repo):**
```powershell
.\tools\flash.ps1
```
Compila e carica `tamagotchi` di default. Opzioni: `-Sketch <nome>`, `-Port COM3`, `-NoMonitor`.

**Con lo script (macOS/Linux, da terminale nella cartella del repo):**
```bash
./tools/flash.sh
```
Compila e carica `tamagotchi` di default. Opzioni: `--sketch <nome>`, `--port /dev/cu.xxx`, `--no-monitor`.

Entrambi compilano lo sketch, lo caricano sulla scheda e aprono il monitor seriale (auto-rilevando
la porta). CTRL+C chiude il monitor.

## Come testare la scheda

- **Windows:** vedi [SETUP.md](SETUP.md)
- **macOS:** vedi [SETUP_MAC.md](SETUP_MAC.md)

Riepilogo rapido (Windows + Arduino IDE):

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
Collega prima l'OLED (vedi [SETUP.md § 8](SETUP.md#8-display-oled-ssd1306-128×64-i²c)), poi apri
`tamagotchi/tamagotchi.ino` e premi **Upload**.
Se resta bloccato su `Connecting....____`, tieni premuto il pulsante **BOOT** finché parte la scrittura.

### 5. Verifica
- Sul display compare una faccina + 3 barre di stato (fame/felicità/energia).
- Premendo un bottone la barra corrispondente cambia subito.
