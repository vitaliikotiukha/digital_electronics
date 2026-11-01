// ============================================================================
// Лабораторна робота №2: Моніторинг пожежної безпеки (NTC + датчик полум'я)
// Рефакторинг: оптимізація структури, усунення запахів коду та дублювання
// ============================================================================

// ----------------------------------------------------------------------------
// Конфігурація пінів (const uint8_t для економії SRAM)
// ----------------------------------------------------------------------------
const uint8_t PIN_NTC       = A0; // Аналоговий вхід терморезистора (NTC)
const uint8_t PIN_FIRE      = 3;  // Цифровий вхід датчика полум'я
const uint8_t PIN_LED_RED   = 5;  // ШІМ-вихід червоного кольору RGB-LED
const uint8_t PIN_LED_GREEN = 6;  // ШІМ-вихід зеленого кольору RGB-LED
const uint8_t PIN_LED_BLUE  = 7;  // Вихід синього кольору RGB-LED

// ----------------------------------------------------------------------------
// Константи налаштувань та порогових значень
// ----------------------------------------------------------------------------
const unsigned long SERIAL_BAUD_RATE   = 9600;
const unsigned long LED_BLINK_DELAY_MS = 500;

const float TEMP_THRESHOLD_WARNING = 25.0f; // Поріг попередження (°C)
const float TEMP_THRESHOLD_DANGER  = 30.0f; // Поріг небезпеки (°C)

// ----------------------------------------------------------------------------
// Стан системи (enum замість кількох розрізнених bool прапорців)
// ----------------------------------------------------------------------------
enum SystemState {
  STATE_SAFE,    // Безпечний стан (біле світло)
  STATE_WARNING, // Попередження: підвищена температура (помаранчеве світло)
  STATE_DANGER   // Небезпека: полум'я або критична температура (червоне світло)
};

// ----------------------------------------------------------------------------
// Прототипи функцій
// ----------------------------------------------------------------------------
void setColor(uint8_t red, uint8_t green, uint8_t blue);
void blinkLed(uint8_t red, uint8_t green, uint8_t blue, unsigned long durationMs = LED_BLINK_DELAY_MS);
void updateLedIndication(SystemState state);
float readNtcTemperature();
SystemState evaluateSystemState(bool fireDetected, float temperature);

// ============================================================================
// Ініціалізація
// ============================================================================
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  pinMode(PIN_FIRE, INPUT);
  pinMode(PIN_NTC, INPUT);

  // Опорна напруга АЦП за замовчуванням (Vref = Vcc)
  analogReference(DEFAULT);
}

// ============================================================================
// Головний цикл програми
// ============================================================================
void loop() {
  // 1. Зчитування даних із сенсорів
  int fireSensorState = digitalRead(PIN_FIRE);
  bool fireDetected   = (fireSensorState == LOW); // Датчик полум'я активний при LOW (0)
  float temperature   = readNtcTemperature();

  // 2. Вивід показників у Serial Monitor
  Serial.print(F("Temp: "));
  Serial.print(temperature);
  Serial.print(F(" °C | Fire sensor state: "));
  Serial.println(fireSensorState);

  // 3. Оцінка поточного рівня безпеки
  SystemState currentState = evaluateSystemState(fireDetected, temperature);

  // 4. Світлова індикація стану
  updateLedIndication(currentState);
}

// ============================================================================
// Реалізація допоміжних функцій
// ============================================================================

// Визначення стану системи на основі показників датчиків
SystemState evaluateSystemState(bool fireDetected, float temperature) {
  if (fireDetected || temperature > TEMP_THRESHOLD_DANGER) {
    return STATE_DANGER;
  } else if (temperature > TEMP_THRESHOLD_WARNING) {
    return STATE_WARNING;
  } else {
    return STATE_SAFE;
  }
}

// Встановлення кольору RGB-світлодіода
void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(PIN_LED_RED, red);
  analogWrite(PIN_LED_GREEN, green);
  analogWrite(PIN_LED_BLUE, blue);
}

// Одноразовий цикл блимання (усуває дублювання delay та setColor)
void blinkLed(uint8_t red, uint8_t green, uint8_t blue, unsigned long durationMs) {
  setColor(red, green, blue);
  delay(durationMs);
  setColor(0, 0, 0);
  delay(durationMs);
}

// Індикація стану через RGB-світлодіод
void updateLedIndication(SystemState state) {
  switch (state) {
    case STATE_SAFE:
      blinkLed(255, 255, 255, LED_BLINK_DELAY_MS); // Білий колір
      break;

    case STATE_WARNING:
      blinkLed(255, 128, 0, LED_BLINK_DELAY_MS);   // Помаранчевий колір
      break;

    case STATE_DANGER:
      blinkLed(255, 0, 0, LED_BLINK_DELAY_MS);     // Червоний колір
      break;
  }
}

// Зчитування та розрахунок температури за формулою Стейнхарта — Харта (B-коефіцієнт)
float readNtcTemperature() {
  const int   ADC_MAX  = 1023;           // 10-бітний АЦП Arduino UNO/Nano (2^10 - 1)
  const float RSER_OHM = 4700.0f;        // 4.7 кОм опір дільника до GND
  const float R0_OHM   = 10000.0f;       // Номінальний опір 10 кОм @ 25°C
  const float BETA     = 3950.0f;        // Коефіцієнт B для MF52AT
  const float T0_K     = 25.0f + 273.15f;// 298.15 K

  int adc = analogRead(PIN_NTC);
  if (adc < 1) {
    // Захист від ділення на нуль при обриві датчика
    adc = 1;
  }

  // Опір терморезистора: Rntc = RSER_OHM * ((ADC_MAX / adc) - 1)
  float rNtc = RSER_OHM * ((float)ADC_MAX / (float)adc - 1.0f);

  // Формула Стейнхарта — Харта: 1/T = 1/T0 + (1/B) * ln(R / R0)
  float invT = (1.0f / T0_K) + (1.0f / BETA) * log(rNtc / R0_OHM);

  // Переведення з Кельвінів у Цельсії
  float tempK = 1.0f / invT;
  return tempK - 273.15f;
}
