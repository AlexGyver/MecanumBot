[![Foo](https://img.shields.io/badge/Version-4.0-brightgreen.svg?style=flat-square)](#versions)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)

# GyverMotor
Библиотека для удобного управления коллекторными моторами через драйвер
- Контроль скорости и направления вращения
- Работа с ШИМ любого разрешения
- Плавный пуск и изменение скорости
- Активный тормоз
- Порог минимального ШИМ
- Deadtime
- Поддержка 5 типов драйверов

> Библиотека "состоит" из двух библиотек: GyverMotor и GyverMotor2. Вторая версия новее, легче, лучше оптимизирована и проще в использовании!

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)
- Для esp8266 (до SDK v2.7.4) не забудь переключить управление в 10 бит!

### Документация
К библиотеке есть [расширенная документация](https://alexgyver.ru/GyverMotor/)

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverMotor** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverMotor/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)

<a id="init"></a>
## Инициализация
Типы драйверов:
- **DRIVER2WIRE** - двухпроводной драйвер с инверсией шим (dir + PWM)
- **DRIVER2WIRE_NO_INVERT** - двухпроводной драйвер без инверсии ШИМ (dir + PWM)
- **DRIVER2WIRE_PWM** - двухпроводной драйвер с двумя ШИМ (PWM + PWM)
- **DRIVER3WIRE** - трёхпроводной драйвер (dir + dir + PWM)
- **RELAY2WIRE** - реле в качестве драйвера (dir + dir)

Для двухпроводных мостовых драйверов стоит отдавать предпочтение типу **DRIVER2WIRE_PWM**. Он требует два ШИМ пина, 
но драйвер работает более правильно и меньше нагружается, а также скорость будет одинаковая в обе стороны. В отличие от простого **DRIVER2WIRE**.
    
<details>
<summary>GyverMotor</summary>

```cpp
// варианты инициализации в зависимости от типа драйвера:
GMotor motor(DRIVER2WIRE, dig_pin, PWM_pin, (LOW / HIGH) );
GMotor motor(DRIVER2WIRE_NO_INVERT, dig_pin, PWM_pin, (LOW / HIGH) );
GMotor motor(DRIVER3WIRE, dig_pin_A, dig_pin_B, PWM_pin, (LOW/HIGH) );
GMotor motor(RELAY2WIRE, dig_pin_A, dig_pin_B, (LOW/HIGH) );
/*
  dig_pin, dig_pin_A, dig_pin_B - любой цифровой пин МК
  PWM_pin - любой ШИМ пин МК
  LOW / HIGH - уровень драйвера. Если при увеличении скорости мотор наоборот тормозит - смени уровень
*/
```
</details>

<details>
<summary>GyverMotor2</summary>

```cpp
GMotor2<тип> motor(пин1, пин2, пин3);               // разрядность ШИМ 8 бит (0.. 255)
GMotor2<тип, разрядность> motor(пин1, пин2, пин3);  // общий случай, разрядность ШИМ в битах

// типы и количество пинов в зависимости от драйвера
GMotor2<DRIVER2WIRE> motor(GPIO, PWM);
GMotor2<DRIVER2WIRE_NO_INVERT> motor(GPIO, PWM);
GMotor2<DRIVER2WIRE_PWM> motor(PWM, PWM);
GMotor2<DRIVER3WIRE> motor(GPIO, GPIO, PWM);
GMotor2<RELAY2WIRE> motor(GPIO, GPIO);
```
</details>

<a id="usage"></a>
## Использование
<details>
<summary>GyverMotor</summary>

```cpp
GMotor(GM_driverType type, int8_t param1 = _GM_NC, int8_t param2 = _GM_NC, int8_t param3 = _GM_NC, int8_t param4 = _GM_NC);
// три варианта создания объекта в зависимости от драйвера:
// GMotor motor(DRIVER2WIRE, dig_pin, PWM_pin, (LOW/HIGH) )
// GMotor motor(DRIVER3WIRE, dig_pin_A, dig_pin_B, PWM_pin, (LOW/HIGH) )
// GMotor motor(RELAY2WIRE, dig_pin_A, dig_pin_B, (LOW/HIGH) )

// установка скорости -255..255 (8 бит) и -1023..1023 (10 бит)
void setSpeed(int16_t duty);

// сменить режим работы мотора:	
// FORWARD - вперёд
// BACKWARD - назад
// STOP - остановить
// BRAKE - активный тормоз
// AUTO - подчиняется setSpeed (-255.. 255)
void setMode(GM_workMode mode);

// направление вращения	
// NORM - обычное
// REVERSE - обратное
void setDirection(bool direction);

// установить минимальную скважность (при которой мотор начинает крутиться)
void setMinDuty(int duty);

// установить разрешение ШИМ в битах
void setResolution(byte bit);

// установить deadtime (в микросекундах). По умолч 0
void setDeadtime(uint16_t deadtime);	

// установить уровень драйвера (по умолч. HIGH)
void setLevel(int8_t level);			

// плавное изменение к указанной скорости (к значению ШИМ)
void smoothTick(int16_t duty);

// скорость изменения скорости
void setSmoothSpeed(uint8_t speed);	

// возвращает -1 при вращении BACKWARD, 1 при FORWARD и 0 при остановке и торможении
int getState();

// внутренняя переменная скважности для отладки
int16_t _duty = 0;

// свовместимость со старыми версиями
// установить выход в 8 бит
void set8bitMode();		

// установить выход в 10 бит
void set10bitMode();
```

### Логика работы
В setMinDuty() можно установить минимальную скорость (0..255), при которой мотор начинает вращение. 
Дальнейшие настройки скорости будут автоматически масштабироваться с учётом минимальной.  
setDirection() задаёт глобальное направление мотора, которое автоматически влияет на все функции скорости.

#### Обычный режим
Запускается setMode(FORWARD) для движения вперёд, setMode(BACKWARD) - назад. 
Скорость устанавливается в setSpeed() либо run(FORWARD/BACKWARD, скорость). Остановить можно setMode(STOP).

#### Авто режим
Запускается setMode(AUTO), скорость задаётся в setSpeed(), поддерживаются отрицательные значения для вращения в другую сторону. 
Остановить можно setMode(STOP).

#### Плавный режим
Для запуска нужно установить setMode(AUTO). В плавном режиме нужно почаще вызывать smoothTick с указанием целевой скорости. При значении 0 мотор сам плавно остановится. 
Для резкой остановки можно использовать setMode(STOP).

</details>

<details>
<summary>GyverMotor2</summary>

```cpp
void setMinDuty(uint16_t mduty);        // установить минимальный ШИМ (умолч. 0)
void setMinDutyPerc(uint16_t mduty);    // установить минимальный ШИМ в % (умолч. 0)
void setDeadtime(uint16_t us);          // установить deadtime в микросекундах (умолч. 0)
void reverse(bool r);                   // реверс направления (умолч. false)

void stop();                            // остановка. Если включен плавный режим, то плавная
void brake();                           // активный тормоз
void setSpeed(int16_t s);               // установить скорость (-макс.. макс)
void setSpeedPerc(int16_t s);           // установить скорость в процентах (-100.. 100%)

int8_t getState();                      // получить статус: мотор крутится (1 и -1), мотор остановлен (0)
int16_t getSpeed();                     // получить текущую скорость мотора

void smoothMode(bool mode);             // установить режим плавного изменения скорости (умолч. false)
void tick();                            // плавное изменение к указанной скорости, вызывать в цикле
void setSmoothSpeed(uint8_t s);         // установить скорость изменения скорости (умолч. 20)
void setSmoothSpeedPerc(uint8_t s);     // установить скорость изменения скорости в процентах
```

### Разрядность ШИМ
В AVR Arduino по умолчанию используется 8-ми битный ШИМ (0.. 255). В esp8266 используется 10-ти битный (0.. 1023). 
При инициализации библиотеки можно настроить нужную разрядность, она может быть любой.

### Скорость
Скорость задаётся в `setSpeed(-макс ШИМ.. макс ШИМ)` в величине ШИМ сигнала, либо в `setSpeedPerc(-100.. 100)` в процентах. Скорость может быть отрицательной, 
тогда мотор будет крутиться в обратную сторону. При значении 0 мотор остановится и драйвер будет отключен.

### Режимы работы
Вызов `stop()` равносилен `setSpeed(0)`. При прямом управлении мотор будет сразу остановлен, при плавном - остановится плавно. Драйвер отключится, вал мотора будет освобождён.  
Вызов `brake()` остановит мотор и переключит драйвер в режим активного торможения (замкнёт мотор через себя). Вал мотора будет сопротивляться вращению.  
Вызов `reverse(true)` инвертирует направление вращения мотора для всех функций.

### Минимальный ШИМ
В `setMinDuty(-макс ШИМ.. макс ШИМ)` можно установить минимальную скорость, при которой мотор начинает вращение, это удобно в большинстве применений. 
Установленная в `setSpeed()` скорость будет автоматически масштабироваться с учётом минимальной. 
Также можно задать минимальную скорость в процентах `setMinDutyPerc(-100.. 100)`.

### Плавный режим
В плавном режиме установленная в `setSpeed()` скорость применяется не сразу, а плавно в течением времени. Для включения 
плавного режима нужно вызвать `smoothMode(true)` и поместить в основном цикле программы функцию-тикер `tick()`. 
Внутри этой функции скорсть будет плавно меняться по встроенному таймеру (период - 50мс). 
Можно настроить скорость изменения скорости - `setSmoothSpeed()` в величинах ШИМ и `setSmoothSpeedPerc()` в процентах.
</details>

<a id="example"></a>
## Пример
Остальные примеры смотри в **examples**!
```cpp
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
```

<a id="versions"></a>
## Версии
- v1.1 - убраны дефайны
- v1.2 - возвращены дефайны
- v2.0:
    - Программный deadtime
    - Отрицательные скорости
    - Поддержка двух типов драйверов и реле
    - Плавный пуск и изменение скорости
- v2.1: небольшие фиксы и добавления
- v2.2: оптимизация
- v2.3: добавлена поддержка esp (исправлены ошибки)
- v2.4: совместимость с другими библами
- v2.5: добавлен тип DRIVER2WIRE_NO_INVERT
- v3.0: переделана логика minDuty, добавлен режим для ШИМ любой битности
- v3.1: мелкие исправления
- v3.2: улучшена стабильность плавного режима
- v3.2.1: вернул run() в public
- v4.0: исправлен баг в GyverMotor. Добавлен GyverMotor2

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!