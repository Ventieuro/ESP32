#!/usr/bin/env python3
"""
img2header.py - converte immagini in un header C con bitmap 1-bit per SSD1306.

Accetta:
  - un GIF animato:            python tools/img2header.py anim.gif
  - una cartella di immagini:  python tools/img2header.py frames/
  - piu' file:                 python tools/img2header.py a.png b.png c.png

Output: un file .h con un array PROGMEM per ogni frame, piu' l'array
`<name>_frames[]` e la costante `<name>_FRAME_COUNT`, pronti per:

    display.drawBitmap(0, 0, <name>_frames[i], <name>_WIDTH, <name>_HEIGHT, SSD1306_WHITE);

Esempi:
  python tools/img2header.py logo.png -o oled_anim/logo.h -n logo
  python tools/img2header.py frames/ -o oled_anim/anim.h -n anim --threshold 110 --invert
  python tools/img2header.py clip.gif -o oled_anim/anim.h -n anim --dither

Richiede Pillow:  pip install Pillow
"""
import argparse
import os
import sys
from pathlib import Path

try:
    from PIL import Image, ImageSequence, ImageOps
except ImportError:
    sys.exit("Manca Pillow. Installa con:  pip install Pillow")

IMG_EXT = {".png", ".bmp", ".jpg", ".jpeg", ".gif", ".webp", ".tif", ".tiff"}


def load_frames(inputs):
    """Ritorna una lista di immagini PIL (una per frame)."""
    frames = []
    paths = []
    for item in inputs:
        p = Path(item)
        if p.is_dir():
            paths.extend(sorted(q for q in p.iterdir() if q.suffix.lower() in IMG_EXT))
        else:
            paths.append(p)

    for p in paths:
        if not p.exists():
            sys.exit(f"File non trovato: {p}")
        im = Image.open(p)
        if getattr(im, "is_animated", False):
            for fr in ImageSequence.Iterator(im):
                frames.append(fr.convert("RGBA"))
        else:
            frames.append(im.convert("RGBA"))
    if not frames:
        sys.exit("Nessun frame da convertire.")
    return frames


def to_mono(im, w, h, threshold, invert, dither, fit):
    """Converte un frame in immagine 1-bit w x h."""
    # appiattisci eventuale trasparenza su sfondo nero
    bg = Image.new("RGBA", im.size, (0, 0, 0, 255))
    im = Image.alpha_composite(bg, im).convert("L")

    if fit == "stretch":
        im = im.resize((w, h), Image.LANCZOS)
    else:  # "contain": mantiene proporzioni, riempie di nero
        im = ImageOps.contain(im, (w, h), Image.LANCZOS)
        canvas = Image.new("L", (w, h), 0)
        canvas.paste(im, ((w - im.width) // 2, (h - im.height) // 2))
        im = canvas

    if dither:
        im = im.convert("1")  # Floyd-Steinberg
    else:
        im = im.point(lambda px: 255 if px >= threshold else 0).convert("1")

    if invert:
        im = ImageOps.invert(im.convert("L")).convert("1")
    return im


def pack_bits(im, w, h):
    """Formato Adafruit drawBitmap: righe, MSB = pixel sinistro, 1 = acceso."""
    row_bytes = (w + 7) // 8
    out = bytearray(row_bytes * h)
    px = im.load()
    for y in range(h):
        for x in range(w):
            if px[x, y]:  # in mode "1": non-zero = bianco = acceso
                out[y * row_bytes + (x >> 3)] |= (0x80 >> (x & 7))
    return out


def emit_header(frames_bytes, w, h, name):
    guard = f"{name.upper()}_H"
    lines = [
        f"// Generato da tools/img2header.py - {len(frames_bytes)} frame, {w}x{h}",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <Arduino.h>",
        "",
        f"#define {name.upper()}_WIDTH  {w}",
        f"#define {name.upper()}_HEIGHT {h}",
        f"#define {name.upper()}_FRAME_COUNT {len(frames_bytes)}",
        "",
    ]
    for i, data in enumerate(frames_bytes):
        hexs = ", ".join(f"0x{b:02X}" for b in data)
        lines.append(f"const unsigned char {name}_frame{i}[] PROGMEM = {{")
        # 16 byte per riga
        toks = [f"0x{b:02X}" for b in data]
        for j in range(0, len(toks), 16):
            lines.append("  " + ", ".join(toks[j:j + 16]) + ",")
        lines.append("};")
        lines.append("")
    arr = ", ".join(f"{name}_frame{i}" for i in range(len(frames_bytes)))
    lines.append(f"const unsigned char* const {name}_frames[] PROGMEM = {{ {arr} }};")
    lines.append("")
    lines.append(f"#endif // {guard}")
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser(description="Immagini/GIF -> header C 1-bit per SSD1306")
    ap.add_argument("inputs", nargs="+", help="GIF, cartella o lista di immagini")
    ap.add_argument("-o", "--output", default="anim.h", help="file .h di uscita")
    ap.add_argument("-n", "--name", default="anim", help="prefisso dei simboli C")
    ap.add_argument("-W", "--width", type=int, default=128)
    ap.add_argument("-H", "--height", type=int, default=64)
    ap.add_argument("--threshold", type=int, default=128, help="soglia 0-255 (senza --dither)")
    ap.add_argument("--invert", action="store_true", help="inverti bianco/nero")
    ap.add_argument("--dither", action="store_true", help="retino Floyd-Steinberg invece della soglia")
    ap.add_argument("--fit", choices=["contain", "stretch"], default="contain",
                    help="contain = mantiene proporzioni (default), stretch = deforma")
    args = ap.parse_args()

    raw = load_frames(args.inputs)
    frames_bytes = []
    for im in raw:
        mono = to_mono(im, args.width, args.height, args.threshold,
                       args.invert, args.dither, args.fit)
        frames_bytes.append(pack_bits(mono, args.width, args.height))

    out = Path(args.output)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(emit_header(frames_bytes, args.width, args.height, args.name))

    kb = sum(len(f) for f in frames_bytes) / 1024
    print(f"OK: {len(frames_bytes)} frame -> {out}  ({kb:.1f} KB in flash)")
    print(f"Nello sketch:  #include \"{out.name}\"")
    print(f"  display.drawBitmap(0, 0, {args.name}_frames[i], "
          f"{args.name.upper()}_WIDTH, {args.name.upper()}_HEIGHT, SSD1306_WHITE);")


if __name__ == "__main__":
    main()
