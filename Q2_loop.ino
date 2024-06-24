void loop() {
  mainKeysHandler();
  if (mainCycleTimer.tick()) {
    ticksActions();
    resetTimeoutActions();
    antiScrActions();

    encoderHandler();
    tumblerHandler();
    funcButtomHandler();

    if (lightCycleTimer.ready()) {
      updateKeys();
      updateWheel();
      updateBackLight();
      updateBPMFlash();
      strip.show();  // вывод
    }
  }
}
