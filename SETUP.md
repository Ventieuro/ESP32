# Setup e primo test ESP32 — guida completa

Guida passo-passo per portare da zero a "scheda funzionante" un kit ESP32 su **Windows**.
Scritta durante il primo avvio reale del kit: include anche i problemi incontrati e come sono stati risolti.

---

## 1. Hardware del kit

| Componente | A cosa serve |
|---|---|
| Modulo di sviluppo **ESP32** (chip USB-seriale **CP2102**) | Il microcontrollore: Wi-Fi + Bluetooth, esegue il firmware |
| **Breadboard** (nell'inserzione tradotta male "lavagna per riunioni") | Prototipazione senza saldare |
| Display **OLED 0,96" 128×64 I²C** (driver SSD1306, pin VCC/GND/SCL/SDA) | Piccolo schermo per testo/grafica |
| Cavo **micro USB** | Alimentazione + dati + programmazione |
| Cavi **jumper** 10x maschio-femmina, 20 cm | Collegamenti scheda ↔ display ↔ sensori |

Scheda: tipo **ESP32 DevKit V1 / DOIT (ESP32-WROOM-32)**. LED rosso = alimentazione, LED blu = GPIO 2.

---

## 2. Software necessario

### Arduino IDE
- Download: <https://www.arduino.cc/en/software>
- Su questo PC era già installato via winget (`ArduinoSA.IDE.stable`).

### Pacchetto (core) ESP32 per Arduino
1. Arduino IDE → **File → Preferenze**
2. Campo *"URL aggiuntive per il Gestore schede"*:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. **Strumenti → Scheda → Gestore schede** → cerca `esp32` → installa **"esp32 by Espressif Systems"**
   - ~250 MB (compilatore xtensa, esptool, librerie del core)
   - Si installa in `C:\Users\<utente>\AppData\Local\Arduino15\packages\esp32\`
   - Versione usata qui: **esp32 3.3.11**

> In alternativa da riga di comando si usa `arduino-cli` (è incluso dentro l'Arduino IDE, in
> `...\resources\app\lib\backend\resources\arduino-cli.exe`):
> ```
> arduino-cli config init --overwrite
> arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
> arduino-cli core update-index
> arduino-cli core install esp32:esp32
> ```

---

## 3. Driver USB-seriale CP2102 (Silicon Labs)  ⚠️ passaggio critico

L'ESP32 di questo kit usa il chip **CP2102**. Windows spesso **non** ha il driver e Windows Update
**non lo trova**. Va installato a mano.

1. Scarica **"CP210x Universal Windows Driver"** da:
   <https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers>
   (file `CP210x_Universal_Windows_Driver.zip`, ~1 MB)
2. Scompatta lo zip in una cartella.
3. Tasto destro su **`silabser.inf`** → **Installa**.
4. Scollega e ricollega l'ESP32.

Verifica in **Gestione dispositivi → Porte (COM e LPT)**:
```
Silicon Labs CP210x USB to UART Bridge (COMx)
```
Annota il numero, es. **COM3**.

---

## 4. Problemi incontrati al primo avvio (e soluzioni)

### Problema A — la scheda si accende ma Windows non la vede
- **Sintomo:** LED rosso acceso, ma nessuna nuova porta COM; in Gestione dispositivi solo
  "Dispositivo USB sconosciuto (richiesta descrittore non riuscita)" o niente del tutto.
- **Causa:** il **cavo micro USB incluso nel kit era di sola ricarica** (2 fili, senza linee dati).
  Il LED rosso si accende comunque perché la corrente passa.
- **Soluzione:** usare un **cavo micro USB "dati"** (di un vecchio telefono/tablet/controller) e/o
  una **porta USB sul retro del PC** (no hub, no porte frontali del case).

### Problema B — compare "CP2102 USB to UART Bridge Controller" con punto esclamativo giallo
- **Sintomo:** in Gestione dispositivi sotto "Altri dispositivi", errore
  `CM_PROB_FAILED_INSTALL`. Windows Update: *"Non è possibile trovare driver per il tuo dispositivo"*.
- **Causa:** manca il driver CP210x.
- **Soluzione:** installare il driver Silicon Labs → vedi **sezione 3**.

### Come diagnosticare "il PC vede la scheda?"
1. Scollega l'ESP32.
2. Registra le porte presenti (PowerShell):
   ```powershell
   Get-PnpDevice -PresentOnly | Where-Object { $_.Class -in 'Ports','USB' } |
     Select-Object -ExpandProperty InstanceId | Sort-Object
   ```
3. Collega l'ESP32, riesegui il comando e confronta: deve comparire una voce nuova
   (`USB\VID_10C4&PID_EA60\...` = CP2102).

---

## 5. Caricare lo sketch di test

Sketch: [`blink_test/blink_test.ino`](blink_test/blink_test.ino) — lampeggia il LED blu (GPIO 2) e
stampa `LED ON` / `LED OFF` sul seriale a 115200 baud.

### Da Arduino IDE
1. **Strumenti → Scheda → ESP32 → "ESP32 Dev Board"**
   (pacchetto ESP32 3.x. Nelle guide più vecchie è chiamata *"ESP32 Dev Module"*: è la stessa scheda, FQBN `esp32:esp32:esp32`)
2. **Strumenti → Porta → COM3** (quella del CP210x)
3. Apri `blink_test/blink_test.ino` → pulsante **Upload** (→)
4. Se resta bloccato su `Connecting....____`: tieni premuto **BOOT** sulla scheda finché parte la
   scrittura, poi rilascia. (Su questa scheda il download automatico ha funzionato senza premere nulla.)

### Da riga di comando (arduino-cli)
```
arduino-cli compile --fqbn esp32:esp32:esp32 blink_test
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 blink_test
```

---

## 6. Verifica finale

- **LED:** il LED blu sulla scheda lampeggia ~1 volta al secondo.
- **Seriale:** Arduino IDE → **Strumenti → Monitor seriale**, baud **115200**. Deve scorrere:
  ```
  ESP32 avviato - test blink
  LED ON
  LED OFF
  LED ON
  ...
  ```

Output reale ottenuto su questo kit (upload OK, `Hash of data verified`, `Hard resetting via RTS pin`):
```
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
...
entry 0x400805b4
ESP32 avviato - test blink
LED ON
LED OFF
LED ON
LED OFF
```

✅ **Scheda funzionante.**

---

## 7. Link utili

| Cosa | Link |
|---|---|
| Arduino IDE | <https://www.arduino.cc/en/software> |
| Core ESP32 (indice pacchetti) | `https://espressif.github.io/arduino-esp32/package_esp32_index.json` |
| Documentazione core ESP32 Arduino | <https://docs.espressif.com/projects/arduino-esp32/en/latest/> |
| Driver CP210x (CP2102) — Silicon Labs | <https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers> |
| Driver CH340 (se una scheda futura monta quel chip) | <https://www.wch-ic.com/downloads/CH341SER_EXE.html> |
| Libreria display OLED SSD1306 (Adafruit) | Gestore librerie Arduino → "Adafruit SSD1306" + "Adafruit GFX" |
| Pinout ESP32 DevKit V1 | <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/> |

---

## 8. Prossimo passo: display OLED

Collegamento I²C tipico ESP32 ↔ OLED:

| OLED | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

Nel codice impostare la risoluzione **128×64** (non 128×32):
```cpp
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
```
Indirizzo I²C del display: di solito `0x3C`.
