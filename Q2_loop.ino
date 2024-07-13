void loop() {
  // Serial.print(analogRead(A1));
  // Serial.print(" ");
  // Serial.print(analogRead(A3));  // pir
  // Serial.print(" ");
  // Serial.println(analogRead(A2));  // photores
  KeysHandlerInGameMode();
  if (mainCycleTimer.tick()) {
    ticksActions();
    resetTimeoutActions();
    antiScrActions();

    encoderHandler();
    tumblerHandler();
    funcButtomHandler();
    mainButtonsHandler();
    PIRHandler();

    if (lightCycleTimer.tick()) {
      updateKeys();
      updateWheel();
      updateBackLight();
      updateBPMFlash();
      updateAlwaysOnMode();
      updateAutoBright();
      strip.show();  // вывод
    }
  }
}
