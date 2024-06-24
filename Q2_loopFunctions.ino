void onPressButton1Actions() {
  b1_flag = 1;
  light_led1 = 255;
  light_on_led1 = 1;
  if_click_todo = 1;
  updateBPMOnClick();
}

void onPressButton2Actions() {
  b2_flag = 1;
  light_led2 = 255;
  light_on_led2 = 1;
  if_click_todo = 1;
  updateBPMOnClick();
}

void onReleaseButton1Actions() {
  anti_scr_led1 = ANTI_SCR;
  b1_flag = 0;
}

void onReleaseButton2Actions() {
  anti_scr_led2 = ANTI_SCR;
  b2_flag = 0;
}

void ticksActions() {
  // кнпоки-крутилки
  // eb.tick();  // оправшивается в encoderHandler
  func_btn.tick();            //
  tumbler_first_btn.tick();   //
  tumbler_second_btn.tick();  //
  btn_1.tick();
  btn_2.tick();
  btn_1_and_2.tick(btn_1, btn_2);
  // таймеры
  //  mainCycleTimer.tick();  // опрашивается в условии в loop
  // lightCycleTimer.tick();  // опрашивается в условии в loop
  timeoutDisableTimer.tick();          //
  upBritnessAfterDisableTimer.tick();  //
  delayBetweenCheckBusyTimer.tick();   //
  inactionDisableBPMTimer.tick();      //
  updateInSleepModeTimer.tick();       //
  dropRxLEDSleepTimer.tick();          //
}

void resetTimeoutActions() {
  if (delay_between_check_busy) {
    if (btn_1.busy() or btn_2.busy() or light_wheel) {  // когда поворачиваем энкодер - light_wheel становится не 0  // это не надо  or func_btn.busy() or tumbler_first_btn.release() or tumbler_second_btn.release()
      resetTimeout();                                   // сброс таймаута
      delay_between_check_busy = 0;
      delayBetweenCheckBusyTimer.start();      // ждём следующую возможность сбросить таймаут
      if (is_pc_in_sleep) is_pc_in_sleep = 0;  // выходим из сна
    }
  }
}


void antiScrActions() {
  if (anti_scr_led1) {
    anti_scr_led1 -= MAIN_CYCLE;
  }

  if (anti_scr_led2) {
    anti_scr_led2 -= MAIN_CYCLE;
  }
}

void updateBPMOnClick() {
  if (IN_GAME_MODE) {
    light_bpm_led_inaction = 255;  // восстановим яркость
    curr_bpm_avg_space = updateBPM();
  }
}
