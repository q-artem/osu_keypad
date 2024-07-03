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
    if (eb.left()) {  // повернули влево
      key_press_btn1 = MEDIA_VOLUME_DOWN;
      key_press_btn2 = CONSUMER_BRIGHTNESS_DOWN;
      wheel_turn = -1;
      last_direction_wheel = 0;
      if (IN_GAME_MODE) data.clicks_wheel_down_in_game += 1;
      else data.clicks_wheel_down_in_work += 1;
    }
    if (eb.right()) {
      key_press_btn1 = MEDIA_VOLUME_UP;
      key_press_btn2 = CONSUMER_BRIGHTNESS_UP;
      wheel_turn = 1;
      last_direction_wheel = 1;
      if (IN_GAME_MODE) data.clicks_wheel_up_in_game += 1;
      else data.clicks_wheel_up_in_work += 1;
    }
    if (btn_1_and_2.pressing()) {
      key_press = key_press_btn1and2;
    } else if (btn_1.pressing()) {
      key_press = key_press_btn1;
    } else if (btn_2.pressing()) {
      key_press = key_press_btn2;
    }
    // действия при любом повороте
    if (!key_press) {                                                            // кнопки 1 и 2 не нажаты
      for (int q = 1 + 2 * eb.fast(); q > 0; --q) Mouse.move(0, 0, wheel_turn);  // крутим колесо
    } else {                                                                     // если кнопка 1 нажата
      if (isFirstActionAfterClick() and IN_GAME_MODE) {                          // если первый раз - убираем последнюю букву
        if (btn_1_and_2.pressing()) Keyboard.write(KEY_BACKSPACE);               // и ещё одну, если жмём две
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.releaseAll();
      }
      for (int q = 1 + 4 * eb.fast(); q > 0; --q) Consumer.write(key_press);  // если быстро крутим - быстро меняем
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
  if (!b1_flag and !digitalRead(3) and !anti_scr_led1) {
    if (IN_GAME_MODE)
      if (data.in_game_keys_mode) Keyboard.press('a');
      else Mouse.click();
    onPressButton1Actions();
  } else if (b1_flag and digitalRead(3)) {
    if (IN_GAME_MODE) Keyboard.release('a');
    onReleaseButton1Actions();
  }

  if (!b2_flag and !digitalRead(5) and !anti_scr_led2) {
    if (IN_GAME_MODE)
      if (data.in_game_keys_mode) Keyboard.press('s');
      else Mouse.click(MOUSE_RIGHT);
    onPressButton2Actions();
  } else if (b2_flag and digitalRead(5)) {
    if (IN_GAME_MODE) Keyboard.release('s');
    onReleaseButton2Actions();
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
  if (func_btn.hasClicks(3)) {  // включаем режим дёрганья мышью
    if (!IN_GAME_MODE) {
      isInAlwaysOnMode = !isInAlwaysOnMode;
      if (isInAlwaysOnMode) alwaysOnModeTimer.start();
      else alwaysOnModeTimer.stop();
    }
  }
  if (func_btn.hasClicks(4)) {
    data.in_game_keys_mode = !data.in_game_keys_mode;
    if (data.in_game_keys_mode) light_led1 = 255; else light_led2 = 255;
  }
  if (func_btn.hasClicks(5)) {
    printStatistics();
  }
  if (func_btn.hasClicks(6)) {
    memory.updateNow();
    strip.fill(mBlack);
    strip.show();
    delay(300);
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
    }
  }
}
