void updateAlwaysOnMode() {
  if (state_always_on == 1) {     // если включена
    if (isInAlwaysOnMode == 0) {  // если надо выключить
      BRIGHT_BPM = constrain(BRIGHT_BPM - LIGHT_CYCLE / 2, 0, 255);
      if (BRIGHT_BPM == 0) {
        state_always_on = 0;
        strip.leds[7] = mBlack;
        BRIGHT_BPM = 255;
      }
    }
  } else {
    if (isInAlwaysOnMode == 1) {  // если надо включить
      if (BRIGHT_BPM == 255) BRIGHT_BPM = 0;
      BRIGHT_BPM = constrain(BRIGHT_BPM + LIGHT_CYCLE / 2, 0, 255);
      if (BRIGHT_BPM == 255) {
        state_always_on = 1;
      }
    }
  }
  if (isInAlwaysOnMode or state_always_on) {
    strip.leds[7] = getFade(mWheel8(counter_always_on++), 255 - BRIGHT_LED_IN_ALWAYS_ON_MODE * BRIGHT_BPM / 255);
  }
}


void updateBackLight() {
  if (!is_pc_in_sleep) {            // для плавной смены режимов
    if (IN_GAME_MODE) {             // если игровой
      if (state_back_light == 1) {  // если были в рабочем режиме
        BRIGHT_BACKLIGHT = constrain(BRIGHT_BACKLIGHT - 2 * LIGHT_CYCLE, 0, 255);
        if (BRIGHT_BACKLIGHT == 0) state_back_light = 0;  // понижаем до упора
      }
      if (state_back_light == 0 and BRIGHT_BACKLIGHT != 255) {
        BRIGHT_BACKLIGHT = constrain(BRIGHT_BACKLIGHT + 2 * LIGHT_CYCLE, 0, 255);  // повышаем до упора
      }
    } else {                        // если рабочий
      if (state_back_light == 0) {  // если были в игровом режиме
        BRIGHT_BACKLIGHT = constrain(BRIGHT_BACKLIGHT - 2 * LIGHT_CYCLE, 0, 255);
        if (BRIGHT_BACKLIGHT == 0) state_back_light = 1;
      }
      if (state_back_light == 1 and BRIGHT_BACKLIGHT != 255) {
        BRIGHT_BACKLIGHT = constrain(BRIGHT_BACKLIGHT + 2 * LIGHT_CYCLE, 0, 255);  // повышаем до упора
      }
    }
  }

  if (state_back_light == 0) {  // если в игровом состоянии
    for (int i = 0; i < 4; i++) {
      // проходим по всей ленте
      // inoise8 вернёт 0-255
      // градиент будет брать значение шума, размер градиента 255 (как максимум шума)
      // i*50 - шаг шума по горизонтали
      // noise_position - общее движение шума по вертикали
      strip.leds[i] = getFade(blueGrad.get(inoise8(i * 50, noise_position[i]), 255), 255 - BRIGHT_BACKLIGHT_IN_GAME_MODE * BRIGHT_BACKLIGHT / 255 * BRIGHT_PHOTOREZ / 255);
      noise_position[i] += 3;
    }
  } else {
    for (int x = 0; x < 2; x++) {
      for (int y = 0; y < 2; y++) {
        // проходим по всей ленте
        // inoise8 вернёт 0-255
        // градиент будет брать значение шума, размер градиента 255 (как максимум шума)
        // i*50 - шаг шума по горизонтали
        // noise_position - общее движение шума по вертикали
        strip.leds[!x and y == 0 ? 2 : !x and y ? 3
                                     : x and y  ? 0
                                                : 1] = getFade(fireGrad.get(inoise8(x * 50 * 3, y * 40 * 3, noise_position[0]), 255), 255 - BRIGHT_BACKLIGHT_IN_WORK_MODE * BRIGHT_BACKLIGHT / 255 * BRIGHT_PHOTOREZ / 255);
        noise_position[0] += 1;
      }
    }
  }
}

void updateWheel() {
  if (light_wheel) {
    if (light_wheel < 0) {
      if (light_wheel < -100) light_wheel = min(light_wheel + speedDownWheel * 10, 0);
      else light_wheel = min(light_wheel + speedDownWheel, 0);
    }
    if (light_wheel > 0) {
      if (light_wheel > 100) light_wheel = max(light_wheel - speedDownWheel * 10, 0);
      else light_wheel = max(light_wheel - speedDownWheel, 0);
    }
    if (last_direction_wheel) {
      strip.leds[6] = getFade(colorWheelUp, 255 - abs(light_wheel) * BRIGHT_WHEEL / 255);
    } else {
      strip.leds[6] = getFade(colorWheelDown, 255 - abs(light_wheel) * BRIGHT_WHEEL / 255);
    }
  }
}

void updateKeys() {
  bool upd1 = 0;
  bool upd2 = 0;
  if (!btn_1.pressing() and light_led1) {
    light_led1 = max(light_led1 - speedDown, 0);
    upd1 = 1;
  }
  if (!btn_2.pressing() and light_led2) {
    light_led2 = max(light_led2 - speedDown, 0);
    upd2 = 1;
  }

  if (light_on_led1) {
    light_on_led1 = 0;
    upd1 = 1;
  }
  if (light_on_led2) {
    light_on_led2 = 0;
    upd2 = 1;
  }

  if (upd1) strip.leds[4] = getFade(colorTap1, 255 - light_led1 * BRIGHT_BUTTONS / 255 * BRIGHT_PHOTOREZ / 255);
  if (upd2) strip.leds[5] = getFade(colorTap2, 255 - light_led2 * BRIGHT_BUTTONS / 255 * BRIGHT_PHOTOREZ / 255);
}

void updateAutoBright() {
  int prev = autobright_level;
  static int last_photores_val = 0;
  if (!light_led1 and !light_led2) last_photores_val = analogRead(A2);
  autobright_level = (31 * autobright_level + 1 * last_photores_val) >> 5;
  if (prev != autobright_level) BRIGHT_PHOTOREZ = constrain(map(autobright_level, MIN_PHOTORESISTOR_VAL, MAX_PHOTORESISTOR_VAL, MIN_REGULATED_BRFIGHT_LEVEL, MAX_REGULATED_BRFIGHT_LEVEL), MIN_REGULATED_BRFIGHT_LEVEL, MAX_REGULATED_BRFIGHT_LEVEL);
  // Serial.print(prev);
  // Serial.print(" ");
  //   Serial.print(analogRead(A2));
  // Serial.print(" ");
  // Serial.println(BRIGHT_PHOTOREZ);  // photores
}