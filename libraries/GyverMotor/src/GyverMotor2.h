#ifndef _GyverMotor2_h
#define _GyverMotor2_h
#include <Arduino.h>

enum GM_driver {
    DRIVER2WIRE,            // двухпроводной драйвер с инверсией шим (dir + PWM)    
    DRIVER2WIRE_NO_INVERT,    // двухпроводной драйвер без инверсии ШИМ (dir + PWM)
    DRIVER2WIRE_PWM,        // двухпроводной драйвер с двумя ШИМ (PWM + PWM)
    DRIVER3WIRE,            // трёхпроводной драйвер (dir + dir + PWM)
    RELAY2WIRE,                // реле в качестве драйвера (dir + dir)
};

#define _GM_SMOOTH_PRD 50   // период таймера плавной скорости, мс

template <GM_driver GM_TYPE, uint8_t GM_RES = 8>
class GMotor2 {
public:
    // инициализация с указанием пинов
    GMotor2(uint8_t pa, uint8_t pb, uint8_t pc = 255) : pinA(pa), pinB(pb), pinC(pc) {
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        if (pinC != 255) pinMode(pinC, OUTPUT);
        setAll(0);
    }
    
    // установить минимальный ШИМ (умолч. 0)
    void setMinDuty(uint16_t mduty) {
        minD = mduty;
    }
    
    // установить минимальный ШИМ в % (умолч. 0)
    void setMinDutyPerc(uint16_t mduty) {
        mduty = constrain(mduty, 0, 100);
        minD = (int32_t)getMax() * mduty / 100;
    }
    
    // реверс направления (умолч. false)
    void reverse(bool r) {
        rev = r ? -1 : 1;
    }
    
    // остановка. Если включен плавный режим, то плавная
    void stop() {
        setSpeed(0);
    }
    
    // активный тормоз
    void brake() {
        setAll(1);
        speed = duty = 0;
    }
    
    // установить скорость (-макс.. макс)
    void setSpeed(int16_t s) {
        speed = s;
        if (!smooth) duty = speed;
        run(duty);
    }
    
    // установить скорость в процентах (-100.. 100%)
    void setSpeedPerc(int16_t s) {
        s = constrain(s, -100, 100);
        setSpeed((int32_t)getMax() * s / 100);
    }

    // установить deadtime в микросекундах (умолч. 0)
    void setDeadtime(uint16_t us) {
        dead = us;
    }
    
    // плавное изменение к указанной скорости, вызывать в цикле
    void tick() {
        if ((speed || duty) && smooth && millis() - tmr >= _GM_SMOOTH_PRD) {
            tmr = millis();
            if (abs(duty - speed) > ds) duty += (duty < speed) ? ds : -ds;
            else duty = speed;
            run(duty);
        }
    }
    
    // установить скорость изменения скорости (умолч. 20)
    void setSmoothSpeed(uint8_t s) {
        ds = s;
    }
    
    // установить скорость изменения скорости в процентах
    void setSmoothSpeedPerc(uint8_t s) {
        s = constrain(s, 0, 100);
        ds = (int32_t)getMax() * s / 100;
    }
    
    // установить режим плавного изменения скорости (умолч. false)
    void smoothMode(bool mode) {
        smooth = mode;
    }
    
    // получить статус: мотор крутится (1 и -1), мотор остановлен (0)
    int8_t getState() {
        return dir;
    }
    
    // получить текущую скорость мотора
    int16_t getSpeed() {
        return duty;
    }
    
private:
    // макс ШИМ при разрешении
    int16_t getMax() {
        return (1 << GM_RES) - 1;
    }
    
    // установка скорости -макс.. макс
    void run(int16_t sp) {
        if (!sp) return setAll(0);      // стоп
        int8_t ndir = (sp > 0) ? rev : -rev;
        if (dead && ndir != dir) {      // направление изменилось
            setAll(0);
            delayMicroseconds(dead);
        }
        dir = ndir;
        int16_t maxD = getMax();
        sp = constrain(sp, -maxD, maxD);
        if (minD) sp = (int32_t)sp * (maxD - minD) >> GM_RES;
        sp = abs(sp) + minD;
        if (GM_RES > 8 && sp == 255) sp++;  // защита от 255 при разрешении > 8 бит

        switch (GM_TYPE) {
        case DRIVER2WIRE:
            digitalWrite(pinA, dir < 0);
            analogWrite(pinB, (dir > 0) ? sp : (maxD - sp));
            break;
            
        case DRIVER2WIRE_NO_INVERT:
            digitalWrite(pinA, dir < 0);
            analogWrite(pinB, sp);
            break;
            
        case DRIVER2WIRE_PWM:
            if (dir > 0) {
                digitalWrite(pinA, 0);
                analogWrite(pinB, sp);
            } else {
                analogWrite(pinA, sp);
                digitalWrite(pinB, 0);
            }
            break;
            
        case DRIVER3WIRE:
            digitalWrite(pinA, dir < 0);
            digitalWrite(pinB, dir > 0);
            analogWrite(pinC, sp);
            break;
            
        case RELAY2WIRE:
            digitalWrite(pinA, dir < 0);
            digitalWrite(pinB, dir > 0);
            break;
        }
    }
    
    // установить все пины
    void setAll(uint8_t val) {
        digitalWrite(pinA, val);
        digitalWrite(pinB, val);
        if (pinC != 255) digitalWrite(pinC, val);
        dir = 0;
    }
    
    const uint8_t pinA, pinB, pinC;
    bool smooth = 0;
    int8_t dir = 0, rev = 1;
    uint8_t dead = 0;
    int16_t minD = 0, speed = 0, duty = 0, ds = 20;
    uint32_t tmr = 0;
};
#endif