void loop() {
  // Serial.print(analogRead(A1));
  // Serial.print(" ");
  // Serial.print(analogRead(A2));  // photores
  // Serial.print(" ");
  // Serial.print(analogRead(A3));  // pir
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
