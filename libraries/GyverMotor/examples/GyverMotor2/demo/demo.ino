#include <GyverMotor2.h>
GMotor2<DRIVER2WIRE> motor(5, 6);
//GMotor2<DRIVER2WIRE_NO_INVERT> motor(5, 6);
//GMotor2<DRIVER2WIRE_PWM> motor(5, 6);
//GMotor2<DRIVER3WIRE> motor(5, 6, 11);
//GMotor2<RELAY2WIRE> motor(5, 6);

void setup() {
  motor.setMinDuty(70);   // мин. ШИМ
  //motor.reverse(1);     // реверс
  //motor.setDeadtime(5); // deadtime
}

void loop() {
  motor.setSpeed(10);
  delay(1000);
  motor.setSpeed(-100);
  delay(1000);
  motor.setSpeed(50);
  delay(1000);
  motor.setSpeed(255);
  delay(1000);
  motor.brake();
  delay(3000);
}
