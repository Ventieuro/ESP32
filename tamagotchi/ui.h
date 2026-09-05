#pragma once
#include <Adafruit_SSD1306.h>
#include "pet.h"

void ui_draw(Adafruit_SSD1306 &display, const PetState &pet);
