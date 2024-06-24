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
      strip.show();  // вывод
    }
  }
}
