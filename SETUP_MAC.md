# Setup e primo test ESP32 — guida macOS

Versione per **macOS** di [SETUP.md](SETUP.md) (che resta la guida di riferimento per Windows).
Stesso kit, stessi sketch: cambiano solo driver, nome porta e script di flash.

---

## 1. Hardware del kit

Vedi [SETUP.md § 1](SETUP.md#1-hardware-del-kit) — identico su Mac.

---

## 2. Software necessario

### Arduino IDE
- Download: <https://www.arduino.cc/en/software> (scegli la versione macOS, Intel o Apple Silicon)

### Pacchetto (core) ESP32 per Arduino
1. Arduino IDE → **Arduino IDE → Impostazioni...** (o **Preferenze**)
2. Campo *"URL aggiuntive per il Gestore schede"*:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. **Strumenti → Scheda → Gestore schede** → cerca `esp32` → installa **"esp32 by Espressif Systems"**
   - Si installa in `~/Library/Arduino15/packages/esp32/`

> In alternativa da riga di comando si usa `arduino-cli`, incluso dentro l'app Arduino IDE:
> ```
> /Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli
> ```
> ```bash
> arduino-cli config init --overwrite
> arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
> arduino-cli core update-index
> arduino-cli core install esp32:esp32
> ```

---

## 3. Driver USB-seriale CP2102 (Silicon Labs)

Su macOS recenti (Big Sur e successivi) il chip **CP2102** spesso viene riconosciuto **senza
installare nulla**. Se dopo aver collegato l'ESP32 non compare nessuna porta nuova:

1. Scarica il **"CP210x VCP Driver for Mac"** da:
   <https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers>
   (pacchetto `.zip` con un installer `.pkg` firmato Apple)
2. Installa il `.pkg`.
3. Su Mac con **Apple Silicon / macOS con System Integrity Protection**, se l'installazione
   chiede di abilitare l'estensione di sistema: **Impostazioni di Sistema → Privacy e Sicurezza**
   → in fondo alla pagina compare un avviso "software di sistema bloccato" → **Consenti**.
4. Riavvia, scollega e ricollega l'ESP32.

Se invece la scheda monta un chip **CH340** (non CP2102), serve il driver a parte:
<https://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html>

### Verifica: la porta è comparsa?
```bash
ls /dev/cu.*
```
Prima di collegare l'ESP32 esegui il comando, poi ricollegalo e riesegui: deve comparire una voce
nuova tipo:
- `/dev/cu.usbserial-XXXX` (CP2102), oppure
- `/dev/cu.wchusbserial-XXXX` (CH340)

Annota il percorso, es. `/dev/cu.usbserial-0001`.

---

## 4. Problemi comuni

### La scheda si accende ma non compare nessuna porta
- **Causa più comune:** cavo micro USB **di sola ricarica** (senza linee dati). Il LED rosso si
  accende comunque perché la corrente passa, ma non compare nessuna `/dev/cu.*` nuova.
- **Soluzione:** usa un cavo micro USB **dati** (di un vecchio telefono/tablet/controller).

### macOS blocca il driver CP210x come "sviluppatore non identificato" o estensione di sistema
- Vai in **Impostazioni di Sistema → Privacy e Sicurezza**, cerca l'avviso relativo a Silicon
  Labs/CP210x e premi **Consenti**, poi riavvia.

---

## 5. Caricare il progetto (tamagotchi)

Sketch: [`tamagotchi/tamagotchi.ino`](tamagotchi/tamagotchi.ino) — richiede l'OLED già collegato
(vedi § 8) e le librerie Adafruit SSD1306/GFX installate, altrimenti resta bloccato in attesa.

### Da Arduino IDE
1. **Strumenti → Scheda → ESP32 → "ESP32 Dev Board"** (FQBN `esp32:esp32:esp32`)
2. **Strumenti → Porta →** la `/dev/cu.usbserial-XXXX` annotata sopra
3. Apri `tamagotchi/tamagotchi.ino` → pulsante **Upload** (→)
4. Se resta bloccato su `Connecting....____`: tieni premuto **BOOT** sulla scheda finché parte la
   scrittura, poi rilascia.

### Da riga di comando (arduino-cli)
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 tamagotchi
arduino-cli upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 tamagotchi
```

### Con lo script `tools/flash.sh`
```bash
./tools/flash.sh                              # sketch tamagotchi, porta auto
./tools/flash.sh --port /dev/cu.usbserial-0001
./tools/flash.sh --no-monitor                 # niente monitor seriale dopo l'upload
```
Compila, carica e apre il monitor seriale. CTRL+C chiude il monitor. Richiede `python3` (già
presente su macOS) per l'auto-rilevamento porta.

---

## 6. Verifica finale

Vedi [SETUP.md § 6](SETUP.md#6-verifica-finale) — identico: faccina + barre di stato sull'OLED,
output seriale a 115200 baud in Arduino IDE → **Strumenti → Monitor seriale**, oppure via
`arduino-cli monitor`.

---

## 7. Link utili

| Cosa | Link |
|---|---|
| Arduino IDE | <https://www.arduino.cc/en/software> |
| Core ESP32 (indice pacchetti) | `https://espressif.github.io/arduino-esp32/package_esp32_index.json` |
| Documentazione core ESP32 Arduino | <https://docs.espressif.com/projects/arduino-esp32/en/latest/> |
| Driver CP210x (CP2102) — Silicon Labs, sezione Mac | <https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers> |
| Driver CH340 per Mac | <https://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html> |
| Libreria display OLED SSD1306 (Adafruit) | Gestore librerie Arduino → "Adafruit SSD1306" + "Adafruit GFX" |
| Pinout ESP32 DevKit V1 | <https://randomnerdtutorials.com/esp32-pinout-reference-gpios/> |

---

## 8. Display OLED SSD1306 128×64 (I²C)

Identico a [SETUP.md § 8](SETUP.md#8-display-oled-ssd1306-128×64-i²c): stesso cablaggio
(GND/VDD→3V3/SCK→GPIO22/SDA→GPIO21), stessa libreria (`Adafruit SSD1306`). Verifica caricando
`tamagotchi` (§ 5): se il display non risponde, il monitor seriale stampa un messaggio d'errore
con indicazioni su cosa controllare.

```bash
arduino-cli lib install "Adafruit SSD1306"
```

---

## 9. Bottoni del tamagotchi

Vedi [SETUP.md § 9](SETUP.md#9-bottoni-del-tamagotchi) — identico: 3 bottoni su GPIO 4/16/17,
ognuno con una gamba a GND (pull-up interno, nessuna resistenza esterna).
