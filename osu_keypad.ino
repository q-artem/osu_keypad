
// Settings
// Visible
#define colorTap 0xFFF000
#define colorTap1 colorTap         // цвет нажатия левой кнопки
#define colorTap2 colorTap         // цвет нажатия правой кнопки
#define colorWheelUp mRed          // прокрутка вверх
#define colorWheelDown mOrange     // прокрутка вниз
#define colorBPM mGreen            // цвет мигания светодиода BPM
#define speedDown 15               // скорость снижения яркости при нажатии
#define speedDownBPM 18            // скорость снижения яркости светодиода BPM
#define speedDownWheel 4           // скорость снижения яркости светодиода колёсика
#define gradientBackColor1 mBlack  // цвета градиента дна
#define gradientBackColor2 mAqua
#define gradientBackColor3 mBlue
#define gradientBackColor4 mNavy
#define TIMEOUT_TO_DISABLE 3600000       // таймаут до отключения подстветки
#define TIME_DISABLING_LIGHT 500         // как долго будет отключаться подстветка
#define TIME_ENABLING_LIGHT 300          // как долго будет включаться подстветка
#define TIMEOUT_INACTION_DISABLE_BPM 30  // как долго будет уменьшаться яроксть светодиода BPM при бездействии
#define PERIOD_BREATH_IN_SLEEP 4000      // период эффекта дыхания когда комп во сне
#define BRIGHT_BREATH_IN_SLEEP 30        // разница между максимальной и минимальной яркостью эффекта дыхания когда комп во сне
#define MIN_BRIGHT_BREATH_IN_SLEEP 3     // минимальная яркость эффекта дыхания когда комп во сне
#define COLOR_BREATH_IN_SLEEP mGreen     // цвет эффекта дыхания когда комп во сне
#define PERIOD_MOUSE_SHAKE 30000         // период дёрганья мыши
#define BRIGHT_LED_IN_ALWAYS_ON_MODE  170  // яркость в режиме дёрганья мышкой
// #define USE_STATIC_COLOR_IN_SLEEP     // если не закомментировано, во сне используется постоянный цвет
#define USE_SIN_CURVE                        // если не закомментировано, во сне используется более резкая смена яркости
#define USE_FAST_CHANGE_WORK_OR_GAME_MOGE 0  // 1 - использовать более быструю смену режимов, 0 - нет
#define BRIGHT_BACKLIGHT_IN_GAME_MODE 30
#define BRIGHT_BACKLIGHT_IN_WORK_MODE 80
// Invisible
#define LIGHT_CYCLE 15     // частота обновления светодиодов 60 герц
#define MAIN_CYCLE 2       // частота обновления всего, кроме кнопок;   ДЕЛИТЕЛЬ ANTI_SCR!
#define ANTI_SCR 36        // миллисекунд, таймаут антидребезга;   КРАТНО MAIN_CYCLE!
#define COL_BUFFER_BPM 10  // величина буфера для подсчёта BPM

// microLED
#define CRT_PGM
#define STRIP_PIN 2  // пин ленты
#define NUMLEDS 8    // кол-во светодиодов
#define COLOR_DEBTH 3
#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
#include <FastLEDsupport.h>  // нужна для шума
int noise_position[4];
mGradient<4> blueGrad;
mGradient<4> fireGrad;

// Кнопочки и крутилочки
#include <EncButton.h>
EncButton eb(7, 8, 11, INPUT_PULLUP);  // колёсико
Button func_btn(15);                   // функциональная кнопка
Button tumbler_first_btn(14);          // тумблер
Button tumbler_second_btn(10);         // тумблер
Button btn_1(3);                       // кнопка 1
Button btn_2(5);                       // кнопка 2
VirtButton btn_1_and_2;                // одновременное нажатие 1 и 2 кнопок

// Таймеры
#include <TimerMs.h>
TimerMs mainCycleTimer(MAIN_CYCLE, 1, 0);
TimerMs lightCycleTimer(LIGHT_CYCLE, 1, 0);
TimerMs timeoutDisableTimer(TIMEOUT_TO_DISABLE, 1, 1);                // время до выключения подстветки
TimerMs inactionDisableBPMTimer(TIMEOUT_INACTION_DISABLE_BPM, 1, 0);  // время полного погасания светодиода BPM
TimerMs upBritnessAfterDisableTimer(1, 0, 0);                         // таймер для асинхронного повышения яркости         (заменить на главный цикл?)
TimerMs delayBetweenCheckBusyTimer(5000, 1, 1);                       // чтобы сильно часто не дёргать таймер таймаута
TimerMs updateInSleepModeTimer(PERIOD_BREATH_IN_SLEEP / 255, 1, 0);   // когда в режиме сна
TimerMs dropRxLEDSleepTimer(5000, 0, 1);                              // сброс некрасивого красного светодиода при отправке компа в сон
TimerMs alwaysOnModeTimer(PERIOD_MOUSE_SHAKE, 0, 0);                  // режим дёрганья мышкой

// Связь с компом
#define HID_CUSTOM_LAYOUT
#define LAYOUT_US_ENGLISH
#include <HID-Project.h>

// Для перезагрузки на всякий случай
#include "GyverWDT.h"

bool IN_GAME_MODE = 0;


// обработка кнопок
bool b1_flag = 0;  // флаги для нажатий
bool b2_flag = 0;
bool if_click_todo = 0;  // если идёт нажатие
int anti_scr_led1 = 0;   // для антидребезга
int anti_scr_led2 = 0;
bool delay_between_check_busy = 1;  // чтобы сильно часто не дёргать таймер таймаута
bool is_turned_wheel = 0;           // для корректрой обработки мультимедиа команд

// иллюминация
int light_led1 = 0;  // яркость светодиода кнопки
int light_led2 = 0;
int light_wheel = 1;            // яркость колёсика
bool last_direction_wheel = 0;  // последнее направление поворота

bool light_on_led1 = 0;  // флаг, нужно включить светодиод один раз при нажатии
bool light_on_led2 = 0;
byte global_britness = 255;  // яркость ленты
// bool is_enable_light = 1;                // для отключения подстветки при простое
byte upBritnessAfterDisableCounter = 0;  // счётчик для асинхронного включения
bool isInAlwaysOnMode = 0;               // режим дёрганья мышкой
bool state_always_on = 0;                // состояние режимa дёрганья мышкой для плавной смены - 0 - выключено, 1 - включено
byte counter_always_on = 0;              // счётчик для радуги
bool is_pc_in_sleep = 0;                 // режим сна
byte pc_in_sleep_light = 0;
byte pc_in_sleep_color = 0;
bool state_back_light = 0;  // состояние задней подсветки для плавной смены - 0 - game mode, 1 - work mode
byte BRIGHT_BACKLIGHT = 255;
byte BRIGHT_WHEEL = 255;
byte BRIGHT_BUTTONS = 255;
byte BRIGHT_BPM = 255;


// рассчёт BPM
unsigned long bpm[COL_BUFFER_BPM];
unsigned long pointsMult4[5];
unsigned long midPointsMult4[4];
unsigned long ostDelFromBPM[COL_BUFFER_BPM];
unsigned long curr_bpm_avg_space = 1000000;
unsigned long time_last_click = 0;
unsigned long bpm_flush_counter = 0;
unsigned long bpm_shift = 0;
bool is_update_bpm = 0;
byte light_bpm_led = 0;
byte light_bpm_led_inaction = 0;
