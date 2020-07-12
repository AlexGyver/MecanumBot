class encCounter {
  public:
    encCounter(byte pin) {
      _pin = pin;
    }
    long update(int direction) {      // приняли 
      bool curState = pinRead(_pin);  // опрос
      if (_lastState != curState) {   // словили изменение
        _lastState = curState;
        if (curState) {               // по фронту
          _counter += direction;      // запомнили поворот
        }
      }
      return _counter;                // вернули
    }

  private:
    long _counter = 0;
    byte _pin;
    bool _lastState = 0;
    // быстрый digitalRead для atmega328
    bool pinRead(uint8_t pin) {
      if (pin < 8) {
        return bitRead(PIND, pin);
      } else if (pin < 14) {
        return bitRead(PINB, pin - 8);
      } else if (pin < 20) {
        return bitRead(PINC, pin - 14);
      }
    }
};
