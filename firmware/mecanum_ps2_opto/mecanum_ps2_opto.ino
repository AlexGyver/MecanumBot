/*
  Скетч к проекту "Телега на всенаправленных колёсах"
  - Страница проекта (схемы, описания): https://alexgyver.ru/mecanumbot
  - Исходники на GitHub: https://github.com/AlexGyver/MecanumBot/
  Проблемы с загрузкой? Читай гайд для новичков: https://alexgyver.ru/arduino-first/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  AlexGyver, AlexGyver Technologies, 2020
  https://www.youtube.com/c/alexgyvershow
  https://github.com/AlexGyver
  https://AlexGyver.ru/
  alex@alexgyver.ru
*/

#define MAX_SPEED 70    // максимальная скорость моторов, в тиках в секунду!
#define MIN_DUTY 50     // мин. сигнал, при котором мотор начинает движение
#define STEP_SIZE 50    // перемещение по кнопкам крестовины (в тиках)
#define ACCEL 7         // ускорение
#define MAX_FOLLOW_SPEED 500  // макс. скорость

// коэффициенты ПИД
#define PID_P 2.2
#define PID_I 0.4
#define PID_D 0.01

// пины энкодеров
#define OPTO_FL 10
#define OPTO_FR 11
#define OPTO_BL 12
#define OPTO_BR 13

#define MOTOR_TEST 0    // тест моторов
// при запуске крутятся ВПЕРЁД по очереди:
// FL - передний левый
// FR - передний правый
// BL - задний левый
// BR - задний правый

// пины драйверов (_B должен быть ШИМ)
#define MOTOR1_A 2
#define MOTOR1_B 3  // ШИМ!
#define MOTOR2_A 4
#define MOTOR2_B 5  // ШИМ!
#define MOTOR3_A 7
#define MOTOR3_B 6  // ШИМ!
#define MOTOR4_A 8
#define MOTOR4_B 9  // ШИМ!

#include <AccelMotor.h>
// тут можно поменять моторы местами
AccelMotor motorBL(DRIVER2WIRE, MOTOR1_A, MOTOR1_B, HIGH);
AccelMotor motorFL(DRIVER2WIRE, MOTOR2_A, MOTOR2_B, HIGH);
AccelMotor motorFR(DRIVER2WIRE, MOTOR3_A, MOTOR3_B, HIGH);
AccelMotor motorBR(DRIVER2WIRE, MOTOR4_A, MOTOR4_B, HIGH);

// пины ресивера ps2
#define PS2_DAT A0
#define PS2_CMD A1
#define PS2_SEL A2
#define PS2_CLK A3

#include <PS2X_lib.h>
PS2X ps2x;

#include "encoder.h"  // мини-класс для щелевого датчика
encCounter encFL(OPTO_FL);
encCounter encFR(OPTO_FR);
encCounter encBL(OPTO_BL);
encCounter encBR(OPTO_BR);

void setup() {
  attachPCINT(OPTO_FL);
  attachPCINT(OPTO_FR);
  attachPCINT(OPTO_BL);
  attachPCINT(OPTO_BR);

  Serial.begin(9600);
  // чуть подразгоним ШИМ https://alexgyver.ru/lessons/pwm-overclock/
  // Пины D3 и D11 - 980 Гц
  TCCR2B = 0b00000100;  // x64
  TCCR2A = 0b00000011;  // fast pwm

  // Пины D9 и D10 - 976 Гц
  TCCR1A = 0b00000001;  // 8bit
  TCCR1B = 0b00001011;  // x64 fast pwm

  // тест моторов
#if (MOTOR_TEST == 1)
  Serial.println("front left");
  motorFL.run(FORWARD, 100);
  delay(3000);
  motorFL.run(STOP);
  delay(1000);
  Serial.println("front right");
  motorFR.run(FORWARD, 100);
  delay(3000);
  motorFR.run(STOP);
  delay(1000);
  Serial.println("back left");
  motorBL.run(FORWARD, 100);
  delay(3000);
  motorBL.run(STOP);
  delay(1000);
  Serial.println("back right");
  motorBR.run(FORWARD, 100);
  delay(3000);
  motorBR.run(STOP);
#endif
  // минимальный сигнал на мотор
  motorFR.setMinDuty(MIN_DUTY);
  motorBR.setMinDuty(MIN_DUTY);
  motorFL.setMinDuty(MIN_DUTY);
  motorBL.setMinDuty(MIN_DUTY);

  motorFR.setMode(AUTO);
  motorBR.setMode(AUTO);
  motorFL.setMode(AUTO);
  motorBL.setMode(AUTO);

  // период интегрирования
  motorFR.setDt(30);
  motorBR.setDt(30);
  motorFL.setDt(30);
  motorBL.setDt(30);

  // режим управления мотора в PID_SPEED
  motorFR.setRunMode(PID_SPEED);
  motorBR.setRunMode(PID_SPEED);
  motorFL.setRunMode(PID_SPEED);
  motorBL.setRunMode(PID_SPEED);

  // ускорение для следования позиции
  motorFR.setAcceleration(ACCEL);
  motorFL.setAcceleration(ACCEL);
  motorBR.setAcceleration(ACCEL);
  motorBL.setAcceleration(ACCEL);

  // макс. скорость для следования позиции
  motorFR.setMaxSpeed(MAX_FOLLOW_SPEED);
  motorFL.setMaxSpeed(MAX_FOLLOW_SPEED);
  motorBR.setMaxSpeed(MAX_FOLLOW_SPEED);
  motorBL.setMaxSpeed(MAX_FOLLOW_SPEED);

  // точность следования к позиции
  motorFR.setStopZone(5);
  motorFL.setStopZone(5);
  motorBR.setStopZone(5);
  motorBL.setStopZone(5);

  // ПИДы
  motorFR.kp = PID_P;
  motorFL.kp = PID_P;
  motorBR.kp = PID_P;
  motorBL.kp = PID_P;

  motorFR.ki = PID_I;
  motorFL.ki = PID_I;
  motorBR.ki = PID_I;
  motorBL.ki = PID_I;

  motorFR.kd = PID_D;
  motorFL.kd = PID_D;
  motorBR.kd = PID_D;
  motorBL.kd = PID_D;

  delay(1000);    // ждём запуска контроллера
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
}

bool currentMode = false; // false скорость, true позиция
long posFR = 0;
long posBR = 0;
long posFL = 0;
long posBL = 0;

// функция переключает между режимом удержания скорости
// и следованием к позиции
void changeMode(bool mode) {
  currentMode = mode;
  if (mode) {
    motorFR.setRunMode(ACCEL_POS);
    motorBR.setRunMode(ACCEL_POS);
    motorFL.setRunMode(ACCEL_POS);
    motorBL.setRunMode(ACCEL_POS);

    // обновляем буфер позиции
    posFR = motorFR.getCurrent();
    posFL = motorFL.getCurrent();
    posBR = motorBR.getCurrent();
    posBL = motorBL.getCurrent();
  } else {
    motorFR.setRunMode(PID_SPEED);
    motorBR.setRunMode(PID_SPEED);
    motorFL.setRunMode(PID_SPEED);
    motorBL.setRunMode(PID_SPEED);
  }
}

void updatePos() {
  motorFR.setTarget(posFR);
  motorBR.setTarget(posBR);
  motorFL.setTarget(posFL);
  motorBL.setTarget(posBL);
}

void loop() {
  static uint32_t tmr;
  if (millis() - tmr >= 50) {
    tmr += 50;
    bool success = ps2x.read_gamepad(false, 0);  // читаем
    ps2x.reconfig_gamepad();      // костыль https://stackoverflow.com/questions/46493222/why-arduino-needs-to-be-restarted-after-ps2-controller-communication-in-arduino

    if (success) {
      if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
        changeMode(true);
        posFR -= STEP_SIZE;
        posFL += STEP_SIZE;
        posBR += STEP_SIZE;
        posBL -= STEP_SIZE;
        updatePos();
      }
      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
        changeMode(true);
        posFR += STEP_SIZE;
        posFL -= STEP_SIZE;
        posBR -= STEP_SIZE;
        posBL += STEP_SIZE;
        updatePos();
      }
      if (ps2x.ButtonPressed(PSB_PAD_UP)) {
        changeMode(true);
        posFR += STEP_SIZE;
        posFL += STEP_SIZE;
        posBR += STEP_SIZE;
        posBL += STEP_SIZE;
        updatePos();
      }
      if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
        changeMode(true);
        posFR -= STEP_SIZE;
        posFL -= STEP_SIZE;
        posBR -= STEP_SIZE;
        posBL -= STEP_SIZE;
        updatePos();
      }

      if (!currentMode) {
        // переводим диапазон 0..255 в -MAX_SPEED..MAX_SPEED
        int valLX = map(ps2x.Analog(PSS_LX), 0, 256, -MAX_SPEED, MAX_SPEED);
        int valLY = map(ps2x.Analog(PSS_LY), 256, 0, -MAX_SPEED, MAX_SPEED); // инвертируем ось
        int valRX = map(ps2x.Analog(PSS_RX), 0, 256, -MAX_SPEED, MAX_SPEED);
        int valRY = map(ps2x.Analog(PSS_RY), 256, 0, -MAX_SPEED, MAX_SPEED); // инвертируем ось

        int dutyFR = valLY + valLX;
        int dutyFL = valLY - valLX;
        int dutyBR = valLY - valLX;
        int dutyBL = valLY + valLX;

        dutyFR += valRY - valRX;
        dutyFL += valRY + valRX;
        dutyBR += valRY - valRX;
        dutyBL += valRY + valRX;

        // ПИД контроль скорости
        motorFR.setTargetSpeed(dutyFR);
        motorBR.setTargetSpeed(dutyBR);
        motorFL.setTargetSpeed(dutyFL);
        motorBL.setTargetSpeed(dutyBL);

        Serial.print(motorFL.getSpeed());
        Serial.print(',');
        Serial.print(motorFL.getDuty());
        Serial.print(',');
        Serial.println(motorFL.getTargetSpeed());
      } else {
        Serial.print(motorFL.getCurrent());
        Serial.print(',');
        Serial.print(motorFL.getDuty());
        Serial.print(',');
        Serial.println(motorFL.getTarget());
      }
    } else {
      // страшно, вырубай
      motorFR.setSpeed(0);
      motorBR.setSpeed(0);
      motorFL.setSpeed(0);
      motorBL.setSpeed(0);
    }

  }
  bool m1 = motorFR.tick(encFR.update(motorFR.getState()));
  bool m2 = motorBR.tick(encBR.update(motorBR.getState()));
  bool m3 = motorFL.tick(encFL.update(motorFL.getState()));
  bool m4 = motorBL.tick(encBL.update(motorBL.getState()));

  // включаем управление скоростью стиками, если все моторы "приехали"
  if (currentMode && !m1 && !m2 && !m3 && !m4) {
    changeMode(false);
  }
}

// вектор PCint
ISR(PCINT0_vect) {  // пины 8-13
  encFR.update(motorFR.getState());
  encBR.update(motorBR.getState());
  encFL.update(motorFL.getState());
  encBL.update(motorBL.getState());
}

// функция для настройки PCINT
uint8_t attachPCINT(uint8_t pin) {
  if (pin < 8) {            // D0-D7 (PCINT2)
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << pin);
    return 2;
  } else if (pin > 13) {    //A0-A5 (PCINT1)
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << pin - 14);
    return 1;
  } else {                  // D8-D13 (PCINT0)
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << pin - 8);
    return 0;
  }
}
