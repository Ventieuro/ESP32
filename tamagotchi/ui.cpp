#include "ui.h"

static const char *moodLabel(Mood m) {
  switch (m) {
    case Mood::Happy:    return "Felice";
    case Mood::Neutral:  return "Ok";
    case Mood::Sad:      return "Triste";
    case Mood::Hungry:   return "Ha fame!";
    case Mood::Sleeping: return "Dorme...";
    case Mood::Dead:     return "R.I.P.";
  }
  return "";
}

static void drawBar(Adafruit_SSD1306 &d, int x, int y, int w, int h, int value) {
  d.drawRect(x, y, w, h, SSD1306_WHITE);
  int fillW = (w - 2) * value / 100;
  if (fillW > 0) d.fillRect(x + 1, y + 1, fillW, h - 2, SSD1306_WHITE);
}

static void drawFace(Adafruit_SSD1306 &d, Mood mood) {
  const int leftX = 44, rightX = 84, eyeY = 22, r = 6;

  if (mood == Mood::Dead) {
    d.drawLine(leftX - r, eyeY - r, leftX + r, eyeY + r, SSD1306_WHITE);
    d.drawLine(leftX - r, eyeY + r, leftX + r, eyeY - r, SSD1306_WHITE);
    d.drawLine(rightX - r, eyeY - r, rightX + r, eyeY + r, SSD1306_WHITE);
    d.drawLine(rightX - r, eyeY + r, rightX + r, eyeY - r, SSD1306_WHITE);
    d.drawLine(leftX, 44, rightX, 44, SSD1306_WHITE);
    return;
  }

  if (mood == Mood::Sleeping) {
    d.drawLine(leftX - r, eyeY, leftX + r, eyeY, SSD1306_WHITE);
    d.drawLine(rightX - r, eyeY, rightX + r, eyeY, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(rightX + r + 4, eyeY - r - 4);
    d.print("z");
    d.setCursor(rightX + r + 10, eyeY - r - 10);
    d.print("Z");
  } else {
    d.drawCircle(leftX, eyeY, r, SSD1306_WHITE);
    d.drawCircle(rightX, eyeY, r, SSD1306_WHITE);
  }

  const int mouthY = 38;
  switch (mood) {
    case Mood::Happy:
      d.drawLine(leftX, mouthY, leftX + 10, mouthY + 6, SSD1306_WHITE);
      d.drawLine(leftX + 10, mouthY + 6, rightX - 10, mouthY + 6, SSD1306_WHITE);
      d.drawLine(rightX - 10, mouthY + 6, rightX, mouthY, SSD1306_WHITE);
      break;
    case Mood::Sad:
      d.drawLine(leftX, mouthY + 6, leftX + 10, mouthY, SSD1306_WHITE);
      d.drawLine(leftX + 10, mouthY, rightX - 10, mouthY, SSD1306_WHITE);
      d.drawLine(rightX - 10, mouthY, rightX, mouthY + 6, SSD1306_WHITE);
      break;
    case Mood::Hungry:
      d.fillCircle((leftX + rightX) / 2, mouthY + 2, 5, SSD1306_WHITE);
      break;
    default:
      d.drawLine(leftX, mouthY + 3, rightX, mouthY + 3, SSD1306_WHITE);
      break;
  }
}

void ui_draw(Adafruit_SSD1306 &display, const PetState &pet) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print(moodLabel(pet_mood(pet)));

  char ageBuf[16];
  snprintf(ageBuf, sizeof(ageBuf), "%lum", pet.ageSeconds / 60);
  int16_t x1, y1; uint16_t w, h;
  display.getTextBounds(ageBuf, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(128 - w, 0);
  display.print(ageBuf);

  drawFace(display, pet_mood(pet));

  display.setCursor(0, 48);
  display.print("F");
  drawBar(display, 10, 48, 118, 6, 100 - pet.hunger);

  display.setCursor(0, 56);
  display.print("H");
  drawBar(display, 10, 56, 60, 6, pet.happiness);

  display.setCursor(74, 56);
  display.print("E");
  drawBar(display, 84, 56, 44, 6, pet.energy);

  display.display();
}
