// открой плоттер на скорости 9600. Команды:
// q<скорость> - скорость в процентах (пример: q0, q50, q90)
// w - stop()
// e - brake()

#include <GyverMotor2.h>
GMotor2<DRIVER2WIRE> motor(5, 6);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(40);
  motor.setMinDuty(70);   // мин. ШИМ
  //motor.reverse(1);     // реверс
  //motor.setDeadtime(5); // deadtime
  motor.smoothMode(1);    // плавный режим
}

void loop() {
  motor.tick();

  // вывод графика
  static uint32_t tmr;
  if (millis() - tmr > 50) {
    tmr = millis();
    Serial.println(motor.getSpeed());
  }

  if (Serial.available() > 0) {
    char key = Serial.read();
    int val = Serial.parseInt();
    switch (key) {
      case 'q': motor.setSpeedPerc(val); break;
      case 'w': motor.stop(); break;
      case 'e': motor.brake(); break;
    }
  }
}
