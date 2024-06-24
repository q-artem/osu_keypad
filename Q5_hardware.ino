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
    }
    if (eb.right()) {
      key_press_btn1 = MEDIA_VOLUME_UP;
      key_press_btn2 = CONSUMER_BRIGHTNESS_UP;
      wheel_turn = 1;
      last_direction_wheel = 1;
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
      if (isFirstActionAfterClick()) {                                           // если первый раз - убираем последнюю букву
        if (btn_1_and_2.pressing()) Keyboard.write(KEY_BACKSPACE);           // и ещё одну, если жмём две
        Keyboard.write(KEY_BACKSPACE);
        Keyboard.releaseAll();
      }
      for (int q = 1 + 4 * eb.fast(); q > 0; --q) Consumer.write(key_press);  // если быстро крутим - быстро меняем
    }
    setWheelLight(last_direction_wheel);
  }
}

void tumblerHandler() {
  if (tumbler_first_btn.press()) {
    IN_GAME_MODE = 1;
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
void mainKeysHandler() {
  if (!b1_flag and !digitalRead(3) and !anti_scr_led1) {
    Keyboard.press('a');
    onPressButton1Actions();
  } else if (b1_flag and digitalRead(3)) {
    Keyboard.release('a');
    onReleaseButton1Actions();
  }

  if (!b2_flag and !digitalRead(5) and !anti_scr_led2) {
    Keyboard.press('s');
    onPressButton2Actions();
  } else if (b2_flag and digitalRead(5)) {
    Keyboard.release('s');
    onReleaseButton2Actions();
  }
}

void funcButtomHandler() {
  if (func_btn.hold()) {
    System.write(SYSTEM_SLEEP);
    dropRxLEDSleepTimer.start();  // спать
    Keyboard.press(MOD_LEFT_ALT);
  }
}