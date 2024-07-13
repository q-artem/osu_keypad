void dropRxLEDSleep() {  // привязана attach к dropRxLEDSleepTimer
  if (!func_btn.holding()) Keyboard.release(MOD_LEFT_ALT);
  timeoutDisableTimer.force();  // спим
}

void encoderHandler() {
  if (eb.tick()) {
    // смотрим куда повернули и ставим переменные
    uint16_t key_press_btn1 = 0;
    uint16_t key_press_btn2 = 0;
    uint16_t key_press_btn1and2 = 1;
    uint16_t key_press = 0;
    int8_t wheel_turn = 0;
    if (eb.right()) {  // повернули влево
      key_press_btn1 = MEDIA_VOLUME_DOWN;
      key_press_btn2 = CONSUMER_BRIGHTNESS_DOWN;
      wheel_turn = -1;
      last_direction_wheel = 0;
      if (IN_GAME_MODE) data.clicks_wheel_down_in_game += 1;
      else data.clicks_wheel_down_in_work += 1;
    }
    if (eb.left()) {
      key_press_btn1 = MEDIA_VOLUME_UP;
      key_press_btn2 = CONSUMER_BRIGHTNESS_UP;
      wheel_turn = 1;
      last_direction_wheel = 1;
      if (IN_GAME_MODE) data.clicks_wheel_up_in_game += 1;
      else data.clicks_wheel_up_in_work += 1;
    }
    if (btn_1_and_2.pressing()) {
      key_press = key_press_btn1and2;
      Keyboard.press(KEY_LEFT_CTRL);
    } else if (btn_1.pressing()) {
      key_press = key_press_btn1;
    } else if (btn_2.pressing()) {
      key_press = key_press_btn2;
    }
    // действия при любом повороте
    if (!key_press) {  // кнопки 1 и 2 не нажаты
                       // for (int q = 1 + 2 * eb.fast(); q > 0; --q) Mouse.move(0, 0, wheel_turn);  // крутим колесо
      if (in_scroll_page_mode) {
        bool is_update_prev = 0;
        if (((wheel_turn < 0) != (curr_speed_scroll < 0)) and (curr_speed_scroll != 0)) {
          is_update_prev = 1;
        }

        if (is_update_prev) curr_speed_scroll += wheel_turn;
        if (!curr_speed_scroll) Mouse.move(0, -wheel_turn * MIN_COL_PIXELS_TO_SCROLL, 0);  // если 0
        else Mouse.move(0, -wheel_turn * abs(curr_speed_scroll), 0);
        if (!is_update_prev) curr_speed_scroll += wheel_turn;
      } else
        for (int q = 1 + 1 * eb.fast(); q > 0; --q) Mouse.move(0, 0, wheel_turn);   // крутим колесо
    } else {                                                                        // если кнопка 1 нажата
      if (isFirstActionAfterClick() and IN_GAME_MODE and data.in_game_keys_mode) {  // если первый раз - убираем последнюю букву
        if (btn_1_and_2.pressing()) Keyboard.write(KEY_BACKSPACE);                  // и ещё одну, если жмём две
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.releaseAll();
      }
      for (int q = 1 + 1 * eb.fast(); q > 0; --q) {
        if (key_press != 1) Consumer.write(key_press);  // если быстро крутим - быстро меняем
        else {
          for (int q = 1 + 1 * eb.fast(); q > 0; --q) Mouse.move(0, 0, wheel_turn);
        }
      }
    }
    setWheelLight(last_direction_wheel);
    is_turned_wheel = 1;
  }
}

void tumblerHandler() {
  if (tumbler_first_btn.press()) {
    IN_GAME_MODE = 1;
    isInAlwaysOnMode = 0;
    // Watchdog.disable();  // куда-то нажали - вырубаем
  }
  if (tumbler_second_btn.press()) {
    IN_GAME_MODE = 0;
    // Watchdog.disable();  // куда-то нажали - вырубаем
  }
  if (!tumbler_first_btn.pressing() and !tumbler_second_btn.pressing()) {
    // Watchdog.enable(RESET_MODE, WDT_PRESCALER_512);  // серединка - перезагрузка
  }
}

void KeysHandlerInGameMode() {
  if (onPressButton1InInterruptFlag) {
    onPressButton1InInterruptFlag = 0;
    onPressButton1Actions();
  }
  if (onReleaseButton1InInterruptFlag) {
    onReleaseButton1InInterruptFlag = 0;
    onReleaseButton1Actions();
  }
  if (onPressButton2InInterruptFlag) {
    onPressButton2InInterruptFlag = 0;
    onPressButton2Actions();
  }
  if (onReleaseButton2InInterruptFlag) {
    onReleaseButton2InInterruptFlag = 0;
    onReleaseButton2Actions();
  }
}

void interruptButton1Handler() {
  if (!b1_flag and !digitalRead(3) and !anti_scr_led1) {
    if (IN_GAME_MODE) {
      if (data.in_game_keys_mode) Keyboard.press('a');
      else Mouse.press(MOUSE_LEFT);
    }
    onPressButton1InInterruptFlag = 1;
  } else if (b1_flag and digitalRead(3)) {
    if (IN_GAME_MODE) {
      if (data.in_game_keys_mode) Keyboard.release('a');
      else Mouse.release(MOUSE_LEFT);
    }
    onReleaseButton1InInterruptFlag = 1;
  }
}

void interruptButton2Handler() {
  if (!b2_flag and !digitalRead(2) and !anti_scr_led2) {
    if (IN_GAME_MODE) {
      if (data.in_game_keys_mode) Keyboard.press('s');
      else Mouse.press(MOUSE_RIGHT);
    }
    onPressButton2InInterruptFlag = 1;
  } else if (b2_flag and digitalRead(2)) {
    if (IN_GAME_MODE) {
      if (data.in_game_keys_mode) Keyboard.release('s');
      else Mouse.release(MOUSE_RIGHT);
    }
    onReleaseButton2InInterruptFlag = 1;
  }
}

void funcButtomHandler() {
  if (func_btn.press()) {
    data.clicks_func_btn += 1;
  }
  if (func_btn.hold(0)) {
    System.write(SYSTEM_SLEEP);
    dropRxLEDSleepTimer.start();  // спать
    Keyboard.press(MOD_LEFT_ALT);
    data.clicks_sleep_pc += 1;
  }
  if (func_btn.hold(2)) {
    Keyboard.write(KEY_ENTER);
    delay(30);
    Keyboard.write(KEY_ENTER);
    delay(300);
    Keyboard.print((1 + 3 * (7 + 9 * (7 * 7 + 2 * 3 * 3))) * 3 * (45634 - 45631) / (3 / 3 * 3 * 3 * 3 / 3 * 3 / 9 * 3 / 3));
    data.clicks_func_btn_unlock_pc += 1;
  }
  if (func_btn.hasClicks(1)) {  // включаем режим прокрутки страницы
    if (!in_scroll_page_mode) {
      in_scroll_page_mode = 1;
      curr_speed_scroll = 0;
      moveMouse(-4500, -4500);
      moveMouse(1700, 540);
      Mouse.click(MOUSE_MIDDLE);
    } else {
      in_scroll_page_mode = 0;
      Mouse.click(MOUSE_MIDDLE);
    }
  }
  if (func_btn.hasClicks(3)) {  // включаем режим дёрганья мышью
    if (!IN_GAME_MODE) {
      isInAlwaysOnMode = !isInAlwaysOnMode;
      if (isInAlwaysOnMode) alwaysOnModeTimer.start();
      else alwaysOnModeTimer.stop();
    }
  }
  if (func_btn.hasClicks(4)) {
    data.in_game_keys_mode = !data.in_game_keys_mode;
    if (data.in_game_keys_mode) light_led1 = 255;
    else light_led2 = 255;
  }
  if (func_btn.hasClicks(5)) {
    printStatistics();
  }
  if (func_btn.hasClicks(6)) {
    memory.updateNow();
    offLight();
    delay(300);
  }
  if (func_btn.hasClicks(7)) {
    memory.updateNow();
    offLight();
    pinMode(15, OUTPUT);  // перезагружаем
    digitalWrite(15, 0);
  }
}

void mainButtonsHandler() {
  if (!IN_GAME_MODE) {
    if (!is_turned_wheel) {
      if (btn_1.busy()) {
        if (btn_1.step(0)) {
          if (btn_1.stepFor(3000)) {
            Keyboard.write(KEY_LEFT_ARROW);
          }
          Keyboard.write(KEY_LEFT_ARROW);
        }
        if (btn_1.hasClicks(1)) Consumer.write(MEDIA_PLAY_PAUSE);
        if (btn_1.hasClicks(2)) Consumer.write(MEDIA_PREVIOUS);
        if (btn_1.hasClicks(3)) Consumer.write(CONSUMER_CALCULATOR);
        if (btn_1.hold(1)) Consumer.write(CONSUMER_BROWSER_BACK);
      }

      if (btn_2.busy()) {
        if (btn_2.step(0)) {
          if (btn_2.stepFor(3000)) {
            Keyboard.write(KEY_RIGHT_ARROW);
          }
          Keyboard.write(KEY_RIGHT_ARROW);
        }
        // if (btn_2.hasClicks(1)) Consumer.write(MEDIA_VOLUME_MUTE);
        if (btn_2.hasClicks(1)) Mouse.click(MOUSE_RIGHT);
        if (btn_2.hasClicks(2)) Consumer.write(MEDIA_NEXT);
        if (btn_2.hasClicks(3)) Consumer.write(CONSUMER_BROWSER_HOME);
        if (btn_2.hold(1)) Consumer.write(CONSUMER_BROWSER_FORWARD);
      }
      if (btn_1_and_2.busy()) {
        if (btn_1_and_2.hasClicks(1)) {
          Keyboard.press(KEY_LEFT_WINDOWS);
          Keyboard.write(KEY_L);
          Keyboard.release(KEY_LEFT_WINDOWS);
          data.clicks_lock_pc += 1;
        }
        if (btn_1_and_2.hold(1)) {
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.press(KEY_LEFT_WINDOWS);
          Keyboard.write(KEY_B);
          Keyboard.release(KEY_LEFT_SHIFT);
          Keyboard.release(KEY_LEFT_CTRL);
          Keyboard.release(KEY_LEFT_WINDOWS);
        }
      }
    }
  }
  if (is_turned_wheel) {
    if (!btn_1.busy() and !btn_2.busy() and !btn_1_and_2.busy()) {
      is_turned_wheel = 0;
      btn_1.action();
      btn_2.action();
      btn_1_and_2.action();
      Keyboard.release(KEY_LEFT_CTRL);
    }
  }
}

void PIRHandler() {
  if (enablePIR) {
    if (digitalRead(A3)) {
      light_wheel = 255;  // вышли из сна
      enablePIR = 0;
      Mouse.move(0, 0, 0);  // пнули комп
    }
  }
}