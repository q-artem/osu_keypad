void loop() {
  if (!b1_flag and !digitalRead(3) and !anti_scr_led1) {
    BootKeyboard.press('a');
    onPressButton1Actions();
  } else if (b1_flag and digitalRead(3)) {
    BootKeyboard.release('a');
    onReleaseButton1Actions();
  }
  if (!b2_flag and !digitalRead(5) and !anti_scr_led2) {
    BootKeyboard.press('s');
    onPressButton2Actions();
  } else if (b2_flag and digitalRead(5)) {
    BootKeyboard.release('s');
    onReleaseButton2Actions();
  }

  if (mainCycleTimer.tick()) {
    ticksActions();
    resetTimeoutActions();
    antiScrActions();

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

    if (eb.turn()) {
      // смотрим куда повернули и ставим переменные
      uint16_t key_press_btn1 = 0;
      uint16_t key_press_btn2 = 0;
      uint16_t key_press_btn1and2 = 1;
      uint16_t key_press = 0;
      int8_t wheel_turn = 0;
      int8_t wheel_wpdate_light = 0;
      if (eb.left()) {  // повернули влево
        key_press_btn1 = MEDIA_VOLUME_DOWN;
        key_press_btn2 = CONSUMER_BRIGHTNESS_DOWN;
        wheel_turn = -1;
        wheel_wpdate_light = 0;
      }
      if (eb.right()) {
        key_press_btn1 = MEDIA_VOLUME_UP;
        key_press_btn2 = CONSUMER_BRIGHTNESS_UP;
        wheel_turn = 1;
        wheel_wpdate_light = 1;
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
          if (btn_1_and_2.pressing()) BootKeyboard.write(KEY_BACKSPACE);           // и ещё одну, если жмём две
          BootKeyboard.write(KEY_BACKSPACE);
          BootKeyboard.releaseAll();
        }
        for (int q = 1 + 4 * eb.fast(); q > 0; --q) Consumer.write(key_press);  // если быстро крутим - быстро меняем
      }
      updateWheelLight(wheel_wpdate_light);
    }

    if (IN_GAME_MODE) {
      if (is_update_bpm) {
        is_update_bpm = 0;
        light_bpm_led_inaction = 255;  // восстановим яркость
        curr_bpm_avg_space = updateBPM(time_last_click);
      }
    }

    if (func_btn.hold()) {
      System.write(SYSTEM_SLEEP);
      System.releaseAll();
      timeoutDisableTimer.force();  // спать
      is_pc_in_sleep = 1;
      dropRxLEDTimer.start();
    }

    changingBackLight();
    if (lightCycleTimer.ready()) {
      if (!b1_state and light_led1) {
        light_led1 = max(light_led1 - speedDown, 0);
        update1();
      }
      if (!b2_state and light_led2) {
        light_led2 = max(light_led2 - speedDown, 0);
        update2();
      }
      if (light_on_led1) {
        light_on_led1 = 0;
        update1();
      }
      if (light_on_led2) {
        light_on_led2 = 0;
        update2();
      }
      if (light_wheel < 0) {
        light_wheel = min(light_wheel + speedDownWheel, 0);
        updateWheel(0);
      }
      if (light_wheel > 0) {
        light_wheel = max(light_wheel - speedDownWheel, 0);
        updateWheel(1);
      }
      updateBackLight();
      updateBPMFlash();
      strip.show();  // вывод
    }
  }
}
