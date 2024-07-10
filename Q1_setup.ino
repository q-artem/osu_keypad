void setup() {
  // стартуем переферию USB
  Consumer.begin();
  Keyboard.begin();
  System.begin();
  Mouse.begin();
  delay(100);
  Consumer.releaseAll();
  Keyboard.releaseAll();
  System.releaseAll();
  Mouse.releaseAll();
  delay(50);
  // инит епрома
  memory.begin(0, 'q');
  // сериал на максимум
  Serial.begin(2000000);
  Serial.setTimeout(0);

  // пинаем генератор случайных чисел
  randomSeed(analogRead(0) + analogRead(1) + analogRead(2) + analogRead(3) + analogRead(4));  // максимальная случайность

  // основные кнопки-крутилки
  // основные кнопки
  pinMode(4, OUTPUT);  // подтяжка
  digitalWrite(4, 0);
  pinMode(3, INPUT_PULLUP);  // входы
  pinMode(2, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  btn_1.setClickTimeout(330);
  btn_1.setHoldTimeout(300);
  btn_1.setDebTimeout(34);
  btn_2.setClickTimeout(330);
  btn_2.setHoldTimeout(300);
  btn_2.setDebTimeout(34);
  btn_1_and_2.setClickTimeout(330);
  btn_1_and_2.setHoldTimeout(300);
  btn_1_and_2.setDebTimeout(34);
  // функциональная кнопка
  pinMode(A0, OUTPUT);  // подтяжка
  digitalWrite(A0, 0);
  // тумблер
  pinMode(16, OUTPUT);  // подтяжка
  digitalWrite(16, 0);
  pinMode(10, INPUT_PULLUP);  // входы
  // колёсико
  pinMode(6, OUTPUT);  // подтяжка
  digitalWrite(6, 0);
  eb.setEncType(EB_STEP2);  // настроим тип
  eb.setEncReverse(1);
  delay(10);                // чуть подождём

  if (digitalRead(10)) {  // обновим текущий режим
    IN_GAME_MODE = 1;
  } else {
    IN_GAME_MODE = 0;
  }

  strip.setBrightness(global_britness);                          // яркость ленты
  timeoutDisableTimer.attach(disablingLight);                    // подключим к таймеру функцию отключения
  upBritnessAfterDisableTimer.attach(upBritnessAfterDisable);    // для асинхронного включения
  delayBetweenCheckBusyTimer.attach(delayBetweenCheckBusyFunc);  // для нормалнього сброса таймаута
  inactionDisableBPMTimer.attach(inactionDisableBPM);            // для уменьшения яркости BPM при бездействии
  updateInSleepModeTimer.attach(updateInSleepMode);              // когда в режиме сна
  dropRxLEDSleepTimer.attach(dropRxLEDSleep);
  alwaysOnModeTimer.attach(alwaysOnMode); // дёргаем мышкой
  countWorkTimeTimer.attach(countWorkTime);  // считаем время работы

  // настроим градиенты
  blueGrad.colors[0] = gradientBackColor1;
  blueGrad.colors[1] = gradientBackColor2;
  blueGrad.colors[2] = gradientBackColor3;
  blueGrad.colors[3] = gradientBackColor4;
  fireGrad.colors[0] = mBlack;
  fireGrad.colors[1] = mRed;
  fireGrad.colors[2] = mYellow;
  fireGrad.colors[3] = mWhite;
  noise_position[1] = random(0, 32000);
  noise_position[2] = random(0, 32000);
  noise_position[3] = random(0, 32000);
  noise_position[4] = random(0, 32000);

  state_back_light = 0;    // это какая то лютая хрень, каждый запуск без этого переменная - любое число
  if (!digitalRead(10)) {  // обновим текущий режим
    state_back_light = 1;
  }
}