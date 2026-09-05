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
| Cavi **jumper** 10x maschio-femmina, 20 cm | Collegamenti scheda ↔ display ↔ bottoni |
| **3 bottoni** (tattili o interruttori) | Input del tamagotchi: nutri / gioca / dormi |

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

## 5. Caricare il progetto (tamagotchi)

Sketch: [`tamagotchi/tamagotchi.ino`](tamagotchi/tamagotchi.ino) — richiede l'OLED già collegato
(vedi **sezione 8**) perché il codice resta bloccato in attesa se non lo trova.

### Da Arduino IDE
1. **Strumenti → Scheda → ESP32 → "ESP32 Dev Board"**
   (pacchetto ESP32 3.x. Nelle guide più vecchie è chiamata *"ESP32 Dev Module"*: è la stessa scheda, FQBN `esp32:esp32:esp32`)
2. **Strumenti → Porta → COM3** (quella del CP210x)
3. Apri `tamagotchi/tamagotchi.ino` → pulsante **Upload** (→)
4. Se resta bloccato su `Connecting....____`: tieni premuto **BOOT** sulla scheda finché parte la
   scrittura, poi rilascia. (Su questa scheda il download automatico ha funzionato senza premere nulla.)

### Da riga di comando (arduino-cli)
```
arduino-cli compile --fqbn esp32:esp32:esp32 tamagotchi
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 tamagotchi
```

---

## 6. Verifica finale

Sul display OLED compare una faccina (occhi + bocca) con 3 barre di stato in basso: **F**
(fame/sazietà), **H** (felicità), **E** (energia). Premendo uno dei 3 bottoni (vedi **sezione 9**)
la barra corrispondente cambia subito e la faccina può cambiare espressione.

Il monitor seriale (**115200 baud**) stampa `Tamagotchi avviato` e poi un messaggio ad ogni
pressione bottone (`Nutrito`, `Giocato`, `Va a dormire`/`Si sveglia`).

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

## 8. Display OLED SSD1306 128×64 (I²C)

**Scollega l'USB mentre cabli.** L'ordine dei pin sul PCB del display varia — leggi le sigle.
Su questo kit il display ha: `GND  VDD  SCK  SDA` (VDD = VCC, SCK = SCL).

| Pin display | Pin ESP32 |
|---|---|
| GND | GND |
| VDD | 3V3  (**non** 5V) |
| SCK | GPIO 22 |
| SDA | GPIO 21 |

### Librerie
```
arduino-cli lib install "Adafruit SSD1306"
```
(tira dentro anche *Adafruit GFX* e *Adafruit BusIO*). Da IDE: Gestore librerie → "Adafruit SSD1306".

### Verifica
Carica `tamagotchi` (sezione 5): se il display non risponde, il monitor seriale (115200 baud)
stampa `Display non trovato: controlla cablaggio e indirizzo I2C` — ricontrolla i collegamenti.
Se il tuo modulo usa l'indirizzo `0x3D` invece di `0x3C`, aggiorna `OLED_ADDR` in
[`tamagotchi/tamagotchi.ino`](tamagotchi/tamagotchi.ino).

Nel codice la risoluzione è **128×64** (non 128×32):
```cpp
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
```

Il display è **monocromatico 1 bit**: pixel acceso (`SSD1306_WHITE`) o spento (`SSD1306_BLACK`),
niente colori né grigi.

---

## 9. Bottoni del tamagotchi

| Bottone | Pin ESP32 |
|---|---|
| Nutri | GPIO 4 |
| Gioca | GPIO 16 |
| Dormi/Sveglia | GPIO 17 |

Ogni bottone: una gamba a **GND**, l'altra al GPIO indicato. Si usa il **pull-up interno**
(`INPUT_PULLUP` nel codice), quindi non serve nessuna resistenza esterna.

Se il tuo cablaggio non ha una fila GND condivisa (es. schede a morsetti a vite con un solo
terminale GND per lato), puoi tranquillamente stringere **più fili insieme sotto lo stesso
morsetto** — a queste tensioni (3.3V, correnti minime) è prassi normale, nessun rischio.
