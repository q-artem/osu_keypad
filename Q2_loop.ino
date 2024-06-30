void loop() {
  KeysHandlerInGameMode();
  if (mainCycleTimer.tick()) {
    ticksActions();
    resetTimeoutActions();
    antiScrActions();

    encoderHandler();
    tumblerHandler();
    funcButtomHandler();
    mainButtonsHandler();

    if (lightCycleTimer.tick()) {
      updateKeys();
      updateWheel();
      updateBackLight();
      updateBPMFlash();
      updateAlwaysOnMode();
      strip.show();  // вывод
    }
  }
}
