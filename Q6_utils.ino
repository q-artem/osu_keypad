void changeAllBritneses(int n) {
  BRIGHT_BACKLIGHT = min(max(BRIGHT_BACKLIGHT + n, 0), 255);
  BRIGHT_WHEEL = min(max(BRIGHT_WHEEL + n, 0), 255);
  BRIGHT_BUTTONS = min(max(BRIGHT_BUTTONS + n, 0), 255);
  BRIGHT_BPM = min(max(BRIGHT_BPM + n, 0), 255);
}

bool isFirstActionAfterClick() {
  if (if_click_todo == 1) {
    if_click_todo = 0;
    return 1;
  }
  return 0;
}

void setWheelLight(bool direction) {
  if (direction) {
    if (light_wheel < 0) light_wheel = -light_wheel;
    light_wheel = min(light_wheel + 50 * (light_wheel == 0 ? 3 : 1), 255);
  } else {
    if (light_wheel > 0) light_wheel = -light_wheel;
    light_wheel = max(light_wheel - 50 * (light_wheel == 0 ? 3 : 1), -255);
  }
}


void updateInSleepMode() {                                                 // привязана attach к updateInSleepModeTimer
  if (is_pc_in_sleep or is_pc_in_sleep == 0 and pc_in_sleep_light != 0) {  // если спим или не спим и ещё не закончили цикл
#ifdef USE_STATIC_COLOR_IN_SLEEP
    strip.leds[7] = getFade(COLOR_BREATH_IN_SLEEP, (long)(255 - pgm_read_byte(&(LinearSin_PGM[pc_in_sleep_light])) * BRIGHT_BREATH_IN_SLEEP / 255 * BRIGHT_BPM / 255 - MIN_BRIGHT_BREATH_IN_SLEEP));
#else
    strip.leds[7] = getFade(mHSV(pc_in_sleep_color, 255, 255), 255 - pgm_read_byte(&(LinearSin_PGM[pc_in_sleep_light])) * BRIGHT_BREATH_IN_SLEEP / 255 * BRIGHT_BPM / 255 - MIN_BRIGHT_BREATH_IN_SLEEP);
#endif
    pc_in_sleep_light++;
    if (!pc_in_sleep_light) pc_in_sleep_color++;
  }
}


void delayBetweenCheckBusyFunc() {  // привязана attach к delayBetweenCheckBusyTimer
  delay_between_check_busy = 1;
}

void offLight() {
  strip.fill(mBlack);
  strip.show();
}

void upBritnessAfterDisable() {  // привязана attach к upBritnessAfterDisableTimer
  upBritnessAfterDisableCounter += 2;
  if (global_britness - upBritnessAfterDisableCounter == 1) upBritnessAfterDisableCounter = global_britness;
  if (upBritnessAfterDisableCounter == global_britness) upBritnessAfterDisableTimer.stop();
  changeAllBritneses(2);
  strip.show();  // вывод
}

void resetTimeout() {
  timeoutDisableTimer.start();
  if (is_pc_in_sleep == 1) {
    upBritnessAfterDisableTimer.setTime(TIME_ENABLING_LIGHT / 2 / global_britness);
    upBritnessAfterDisableTimer.start();
    upBritnessAfterDisableCounter = 0;
    is_pc_in_sleep = 0;
  }
}

void disablingLight() {  // привязана attach к timeoutDisableTimer
  is_pc_in_sleep = 1;
  for (int i = global_britness; i > -1; --i) {
    strip.setBrightness(i);  // ну да, костыль
    strip.show();            // вывод
    delay(TIME_DISABLING_LIGHT / global_britness);
  }
  changeAllBritneses(-255);
  strip.setBrightness(global_britness);
  light_bpm_led_inaction = 0;  // отрубим BPM
  cleanBPMBuffer();            // чтоб глаз не мозолило
  BRIGHT_BPM = 255;
#ifndef USE_STATIC_COLOR_IN_SLEEP
  pc_in_sleep_color = random(0, 256);
#endif
  delayBetweenCheckBusyTimer.force();  // чтоб можно было сразу проснуться
  timeoutEnablePIRTimer.start();  // таймер на включение ПИР
}

void cleanBPMBuffer() {
  for (int i = 0; i < COL_BUFFER_BPM; i++) {
    bpm[i] = 0;
  }
}

void inactionDisableBPM() {  // привязана attach к inactionDisableBPMTimer
  if (light_bpm_led_inaction) {
    --light_bpm_led_inaction;
    if (IN_GAME_MODE == 0) {
      light_bpm_led_inaction = max(light_bpm_led_inaction - 5, 0);
    }
  }
  if (light_bpm_led_inaction == 1) {
    cleanBPMBuffer();
  }
}

void updateBPMFlash() {
  if (is_pc_in_sleep == 0 and pc_in_sleep_light == 0) {
    bpm_flush_counter += LIGHT_CYCLE;
    if (bpm_flush_counter > bpm_shift + curr_bpm_avg_space) {
      bpm_flush_counter = bpm_shift;
      light_bpm_led = 255;
    }
    if (light_bpm_led) {
      light_bpm_led = max(light_bpm_led - speedDownBPM, 0);
    }
    strip.leds[7] = getFade(colorBPM, (long)(255 - light_bpm_led * light_bpm_led_inaction / 255 * BRIGHT_BPM / 255));
  }
}

void countWorkTime() {  // привязана attach к countWorkTimeTimer
  if (is_pc_in_sleep) {
    data.all_sleep_time += 1;
  } else {
    if (isInAlwaysOnMode) {
      data.all_always_on_mode_time += 1;
    } else {
      if (IN_GAME_MODE) {
        data.all_game_time += 1;
      } else {
        data.all_work_time += 1;
      }
    }
  }
}

void alwaysOnMode() {  // привязана attach к alwaysOnModeTimer
  Mouse.move(-1, 0, 0);
  Mouse.move(1, 0, 0);
  resetTimeout();  // не спать
}

void timeoutEnablePIR() {  // привязана attach к timeoutEnablePIRTimer
  enablePIR = 1;
}

void printStatistics() {
  data.show_statistics_amount += 1;
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_RIGHT_ALT);
  Keyboard.release(KEY_RIGHT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  delay(100);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  delay(100);
  Keyboard.press(KEY_LEFT_WINDOWS);
  Keyboard.write(KEY_R);
  Keyboard.release(KEY_LEFT_WINDOWS);
  delay(100);
  Keyboard.print("notepad");
  Keyboard.write(KEY_ENTER);
  delay(300);
  Serial.println(data.clicks_btn1_in_game);
  Serial.println(data.clicks_btn1_in_work);
  Serial.println(data.clicks_btn2_in_game);
  Serial.println(data.clicks_btn2_in_work);
  Serial.println(data.clicks_wheel_down_in_game);
  Serial.println(data.clicks_wheel_up_in_game);
  Serial.println(data.clicks_wheel_down_in_work);
  Serial.println(data.clicks_wheel_up_in_work);
  Serial.println(data.clicks_func_btn);
  Serial.println(data.clicks_func_btn_unlock_pc);
  Serial.println(data.clicks_lock_pc);
  Serial.println(data.clicks_sleep_pc);
  Serial.println(data.show_statistics_amount);
  Serial.println(data.all_work_time);
  Serial.println(data.all_game_time);
  Serial.println(data.all_sleep_time);
  Serial.println(data.all_always_on_mode_time);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("     OOOOOOOOO       sssssssssss"));
  delay(6500);
  Keyboard.print(F(" / ___ \\              | |  / )             (_____ \\          | |\n"));
  delay(650);
  Keyboard.print(F("| |   | | ____  _   _ | | / /  ____  _   _  _____) )____   _ | |\n"));
  delay(650);
  Keyboard.print(F("| |   | |/ ___|| | | || |< <  / _  )| | | ||  ____// _  | / || |\n"));
  delay(650);
  Keyboard.print(F("| |___| |\\___ \\| |_| || | \\ \\( (/ / | |_| || |    ( ( | |( (_| |\n"));
  delay(650);
  Keyboard.print(F(" \\_____/ |____/ \\____||_|  \\_)\____) \\__  ||_|     \\_||_| \\____|\n"));
  delay(650);
  Keyboard.print(F("                                    (____/                      \n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("                          Staticstics\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("|    Значение\режим     | GAME MODE  | WORK MODE  |    ALL     |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Кнопка 1, кликов      |            |            |            |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Кнопка 2, кликов      |            |            |            |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Функц. кнопка, кликов | ========== | ========== |            |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Колёсико, тиков вниз  |            |            |            |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Колёсико, тиков вверх |            |            |            |\n"));
  delay(650);
  Keyboard.print(F("|-----------------------+------------+------------+------------|\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("|------------------------+------------|\n"));
  delay(650);
  Keyboard.print(F("|        Значение        | Количество |\n"));
  delay(650);
  Keyboard.print(F("|------------------------+------------|\n"));
  delay(650);
  Keyboard.print(F("| Разблокировок ПК, раз  |            |\n"));
  delay(650);
  Keyboard.print(F("| Блокировок ПК, раз     |            |\n"));
  delay(650);
  Keyboard.print(F("| Отправок ПК в сон, раз |            |\n"));
  delay(650);
  Keyboard.print(F("| Выводов стат., раз     |            |\n"));
  delay(650);
  Keyboard.print(F("|------------------------+------------|\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  Keyboard.print(F("                        Время работы\n"));
  delay(650);
  Keyboard.print(F("\n"));
  delay(650);
  // Keyboard.print(F("|-----------------------------+-----------------------------|\n"));
  //  delay(650);
  // Keyboard.print(F("|            Режим            |         Время работы        |\n"));
  //  delay(650);
  // Keyboard.print(F("|-----------------------------+-----------------------------|\n"));
  //  delay(650);
  // Keyboard.print(F("| GAME MODE                   |                             |\n"));
  //  delay(650);
  // Keyboard.print(F("| WORK MODE                   |                             |\n"));
  //  delay(650);
  // Keyboard.print(F("| Режим ожидания              |                             |\n"));
  //  delay(650);
  // Keyboard.print(F("| Режим поддержания работы ПК |                             |\n"));
  //  delay(650);
  // Keyboard.print(F("| Текущий сеанс               |                             |\n"));
  //  delay(650);
  // Keyboard.print(F("|-----------------------------+-----------------------------|\n"));
  //  delay(650);
  // Keyboard.print(F("\n"));
  //  delay(650);
  // Keyboard.print(F("                                                 G-take Corp. 2024\n"));
  //  delay(650);
  // Keyboard.print(F("\n"));
}

void enterDigit() {
}

void moveMouse(int dist_x, int dist_y) {
  char reverse_x = 1;
  char reverse_y = 1;
  if (dist_x < 0) {
    reverse_x = -1;
    dist_x = -dist_x;
  }
  if (dist_y < 0) {
    reverse_y = -1;
    dist_y = -dist_y;
  }
  while (dist_x != 0 or dist_y != 0) {
    char cr_x = 0;
    char cr_y = 0;
    if (dist_x > 120) {
      cr_x = 120;
      dist_x -= 120;
    } else {
      cr_x = dist_x;
      dist_x = 0;
    }
    if (dist_y > 120) {
      cr_y = 120;
      dist_y -= 120;
    } else {
      cr_y = dist_y;
      dist_y = 0;
    }
    Mouse.move(reverse_x * cr_x, reverse_y * cr_y, 0);
  }
}

// unsigned long updateBPM(unsigned long curr) {
//   for (int i = 0; i < 9; i++) {
//     bpm[i] = bpm[i + 1];
//   }
//   bpm[9] = curr;
//   unsigned long min_delta = 2000000;
//   for (int i = 0; i < 9; i++) {
//     min_delta = min(min_delta, bpm[i + 1] - bpm[i]);
//   }
//   unsigned long avg_space = 0;
//   for (int i = 0; i < 9; i++) {
//     int min_w = 0;
//     unsigned long min_delta_del_w = 2000000;
//     for (int w = 1; w < 8; w++) {
//       if (abs(min_delta - (bpm[i + 1] - bpm[i]) / w) < min_delta_del_w) {
//         min_delta_del_w = abs(min_delta - (bpm[i + 1] - bpm[i]) / w);
//         min_w = w;
//       }
//     }
//     avg_space += (long)((bpm[i + 1] - bpm[i]) / min_w);
//   }
//   avg_space /= 9;
//   bpm_shift = bpm[0] % avg_space;
//   return avg_space;
// }


unsigned long BPMToMillis(unsigned long n) {
  return (unsigned long)1000 * 60 / n;
}

unsigned long millisToBPM(unsigned long n) {
  return (unsigned long)1000 * 60 / n;
}


unsigned long findScorePointOnBPM(unsigned long note) {
  note = note * 4;
  unsigned long one_step = midPointsMult4[0] / 2;
  if (note > pointsMult4[0]) {
    if (note < pointsMult4[1]) {
      if (midPointsMult4[0] < note) {
        return (one_step - (note - midPointsMult4[0])) * 2;  // растянем в 2 раза, т. к. маленький
      } else {
        return (2 * one_step - (midPointsMult4[0] - note));
      }
    } else {
      if (note < pointsMult4[2]) {
        if (midPointsMult4[1] < note) {
          return (2 * one_step - (note - midPointsMult4[1]));
        } else {
          return (one_step - (midPointsMult4[1] - note)) * 2;  // растянем в 2 раза, т. к. маленький
        }
      } else {
        if (note < pointsMult4[3]) {
          if (midPointsMult4[2] < note) {
            return (2 * one_step - (note - midPointsMult4[2]));
          } else {
            return (2 * one_step - (midPointsMult4[2] - note));
          }
        } else {
          if (note < pointsMult4[4]) {
            if (midPointsMult4[3] < note) {
              return (2 * one_step - (note - midPointsMult4[3]));
            } else {
              return (2 * one_step - (midPointsMult4[3] - note));
            }
          } else {
            return 0;  // игнорируем
          }
        }
      }
    }
  }
}


unsigned long updateBPM() {
  for (int i = 0; i < COL_BUFFER_BPM - 1; i++) {
    bpm[i] = bpm[i + 1];
  }
  bpm[COL_BUFFER_BPM - 1] = millis();  // сдвинули на 1 и записали текущий новый
  // пересчёт среднего
  unsigned long average_delta = 0;
  int cntr = 0;
  for (int i = 0; i < COL_BUFFER_BPM - 1; i++) {
    if (bpm[i]) {
      average_delta += bpm[i + 1] - bpm[i];
      cntr++;
    }
  }
  average_delta = max(average_delta / cntr, millisToBPM(600));  // максимум 600 BPM
  // int start = max(millisToBPM(average_delta) / 2 - 1, 1);
  // int end = millisToBPM(average_delta) / 2 + millisToBPM(average_delta) + 1;
  int start = 0;
  start = max(millisToBPM(average_delta) - 20, 1);
  int end = 0;
  end = millisToBPM(average_delta) + 20;
  // начинаем перебор значений BPM
  unsigned long score = 0;
  unsigned long max_score = 0;
  int best_bpm = 1;
  if (start < 1) start = 1;  // непонятно почему max не работает
  // Serial.println(33333);
  // Serial.println(average_delta);
  // Serial.println(start);
  // Serial.println(end);
  // Serial.println(33333);
  // Serial.println(micros()); // 27 мс
  for (int curr_bpm = start; curr_bpm < end + 1; ++curr_bpm) {  // перебор bpm
    score = 0;
    // считаем граничные точки
    unsigned long bpm_in_millis = BPMToMillis(curr_bpm);
    pointsMult4[0] = 0;
    pointsMult4[1] = 3 * bpm_in_millis;
    pointsMult4[2] = 6 * bpm_in_millis;
    pointsMult4[3] = 10 * bpm_in_millis;
    pointsMult4[4] = 14 * bpm_in_millis;
    midPointsMult4[0] = 2 * bpm_in_millis;
    midPointsMult4[1] = 4 * bpm_in_millis;
    midPointsMult4[2] = 8 * bpm_in_millis;
    midPointsMult4[3] = 12 * bpm_in_millis;
    // перебор значений
    for (int i = 0; i < COL_BUFFER_BPM - 1; i++) {
      if (bpm[i] == 0) continue;
      // unsigned long curr_delta = bpm[i + 1] - bpm[i];
      score += findScorePointOnBPM(bpm[i + 1] - bpm[i]) * 1000 / (midPointsMult4[0] / 2);  // коррекция
    }
    if (score > max_score) {
      best_bpm = (unsigned long)curr_bpm;
      max_score = score;
    }
  }
  // Serial.println(micros());
  // лучший bpm лежит на этот момент в best_bpm
  unsigned long best_bpm_in_millis = BPMToMillis(best_bpm);
  // теперь перебираем все сдвиги, ищем наилучший
  int step_to_find_bpm = best_bpm_in_millis > 800 ? (best_bpm_in_millis > 2500 ? (best_bpm_in_millis > 10000 ? (72) : 12) : 3) : 1;  // чтобы перебрать максимум 800 значений

  max_score = 4000000000;
  unsigned long best_offset = 0;


  // average_delta = 0;
  // for (int i = 0; i < COL_BUFFER_BPM; i++) {
  //   if (bpm[i]) {
  //     average_delta += bpm[i] / COL_BUFFER_BPM;
  //   }
  // }
  // average_delta = max(average_delta / cntr, millisToBPM(600));  // максимум 600 BPM

  for (int i = 0; i < COL_BUFFER_BPM; i++) {
    if (bpm[i] == 0) {
      ostDelFromBPM[i] = 0;
    } else {
      ostDelFromBPM[i] = (bpm[i]) % (best_bpm_in_millis >> 1);
    }
  }

  average_delta = 0;
  cntr = 0;
  for (int i = 0; i < COL_BUFFER_BPM; i++) {
    if (ostDelFromBPM[i]) {
      cntr++;
    }
  }
  for (int i = 0; i < COL_BUFFER_BPM; i++) {
    if (ostDelFromBPM[i]) {
      average_delta += ostDelFromBPM[i] / cntr;
    }
  }

  // это всё сильно долго
  // Serial.println(best_bpm_in_millis);
  // Serial.println(step_to_find_bpm);
  // for (int offset = 0; offset < best_bpm_in_millis / 2 + 1 + step_to_find_bpm; offset += step_to_find_bpm) {  // для проврки bpm хватит и половины
  //   for (int i = 0; i < COL_BUFFER_BPM; i++) {                                                                // остатки от деления на половинку BPM
  //     if (bpm[i] == 0) {
  //       ostDelFromBPM[i] = 0;
  //     } else {
  //       ostDelFromBPM[i] = (bpm[i] + offset) % (best_bpm_in_millis >> 1);
  //     }
  //   }
  //   score = 0;
  //   for (int i = 0; i < COL_BUFFER_BPM; i++) {
  //     score += ostDelFromBPM[i];
  //   }
  //   if (score < max_score) {
  //     best_offset = best_bpm_in_millis - offset;
  //     max_score = score;
  //   }
  // }
  // и лучший оффсет теперь в best_offset
  bpm_flush_counter -= bpm_shift;
  bpm_shift = average_delta;
  bpm_flush_counter += bpm_shift;
  return best_bpm_in_millis;
}