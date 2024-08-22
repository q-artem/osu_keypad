void loop() {
  // Serial.print(analogRead(A0));
  // Serial.print(" ");
  // Serial.print(analogRead(A1));  // pir
  // Serial.print(" ");
  // Serial.println(analogRead(A2));  // photores
  SensorKeysHandler();
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
