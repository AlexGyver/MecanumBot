// потенциометр на A0

#include <GyverMotor2.h>
GMotor2<DRIVER2WIRE> motor(5, 6);

void setup() {
  motor.setMinDuty(70);   // мин. ШИМ
  //motor.reverse(1);     // реверс
  //motor.setDeadtime(5); // deadtime
}

void loop() {
  // переводим в диапазон -255.. 255
  int val = analogRead(0) / 2 - 512;
  motor.setSpeed(val);
  delay(100);
}
