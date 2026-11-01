// Піни керування
const int PIN_BTN_RESET   = 2;  // Кнопка Reset (до GND)
const int PIN_TRIG_R      = 3;  // Вихід скидання на лінію R тригера

// Піни моніторингу виходів тригера
const int PIN_SENSE_Q     = 8;  // Вхід виходу Q
const int PIN_SENSE_NQ    = 9;  // Вхід виходу !Q

// Пін аварійної індикації
const int PIN_LED_INVALID = 12; // Світлодіод забороненого стану на боці Arduino

void setup() {
  Serial.begin(115200);

  // Налаштування кнопки скидання з внутрішньою підтяжкою до +5V
  pinMode(PIN_BTN_RESET, INPUT_PULLUP);

  // Пін скидання тригера: у спокої завжди LOW (0)
  pinMode(PIN_TRIG_R, OUTPUT);
  digitalWrite(PIN_TRIG_R, LOW);

  // Входи зчитування стану тригера
  pinMode(PIN_SENSE_Q, INPUT);
  pinMode(PIN_SENSE_NQ, INPUT);

  // Світлодіод забороненого стану
  pinMode(PIN_LED_INVALID, OUTPUT);
  digitalWrite(PIN_LED_INVALID, LOW);
}

void loop() {
  // 1. Апаратне скидання тригера за натисканням кнопки
  if (digitalRead(PIN_BTN_RESET) == LOW) {
    digitalWrite(PIN_TRIG_R, HIGH);
  } else {
    digitalWrite(PIN_TRIG_R, LOW);
  }

  // 2. Зчитування поточного стану тригера
  int stateQ  = digitalRead(PIN_SENSE_Q);
  int stateNQ = digitalRead(PIN_SENSE_NQ);

  // 3. Перевірка на заборонений стан для NOR (активний при Q == 0 та !Q == 0)
  int invalidState = (stateQ == LOW && stateNQ == LOW) ? 1 : 0;
  digitalWrite(PIN_LED_INVALID, invalidState ? HIGH : LOW);

  // 4. Вивід даних для Serial Plotter
  Serial.print("Q:");
  Serial.print(stateQ);
  Serial.print(" ");
  Serial.print("not_Q:");
  Serial.print(stateNQ);
  Serial.print(" ");
  Serial.print("Invalid_State:");
  Serial.println(invalidState);

  delay(40); // Частота дискретизації для зручного масштабу в плотері
}