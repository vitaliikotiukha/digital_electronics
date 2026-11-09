const int PIN_NTC   = A0; // Аналоговий вхід терморезистора
const int PIN_RED   = 5;  // Вихід червоного кольору RGB-світлодіода
const int PIN_GREEN = 6;  // Вихід зеленого кольору RGB-світлодіода
const int PIN_BLUE  = 7;  // Вихід синього кольору RGB-світлодіода
const int PIN_FIRE  = 3;  // Цифровий вхід датчика полум'я

const unsigned long SERIAL_BAUD        = 9600; // Швидкість Serial-порту
const unsigned long LED_BLINK_DELAY_MS = 500;  // Затримка блимання світлодіода (мс)


const int COLOR_OFF_R = 0;
const int COLOR_OFF_G = 0;
const int COLOR_OFF_B = 0;

// білий колір
const int COLOR_SAFE_R = 255;
const int COLOR_SAFE_G = 255;
const int COLOR_SAFE_B = 255;

// помаранчевий колір
const int COLOR_WARNING_R = 255;
const int COLOR_WARNING_G = 128;
const int COLOR_WARNING_B = 0;

// червоний колір
const int COLOR_DANGER_R = 255;
const int COLOR_DANGER_G = 0;
const int COLOR_DANGER_B = 0;


const float TEMP_WARNING_THRESHOLD = 25.0; // Поріг попередження
const float TEMP_DANGER_THRESHOLD  = 30.0; // Поріг небезпеки

const float KELVIN_OFFSET = 273.15; // Зсув абсолютної температури

float temp = 0;

bool isDanger  = false;
bool isWarning = false;
bool isSafe    = false;

void LED(bool danger, bool warning, bool safe) {
  if (safe) {
    setColor(COLOR_SAFE_R, COLOR_SAFE_G, COLOR_SAFE_B);
    delay(LED_BLINK_DELAY_MS);
    setColor(COLOR_OFF_R, COLOR_OFF_G, COLOR_OFF_B);
    delay(LED_BLINK_DELAY_MS);
  }
  else if (warning) {
    setColor(COLOR_WARNING_R, COLOR_WARNING_G, COLOR_WARNING_B);
    delay(LED_BLINK_DELAY_MS);
    setColor(COLOR_OFF_R, COLOR_OFF_G, COLOR_OFF_B);
    delay(LED_BLINK_DELAY_MS);
  }
  else if (danger) {
    setColor(COLOR_DANGER_R, COLOR_DANGER_G, COLOR_DANGER_B);
    delay(LED_BLINK_DELAY_MS);
    setColor(COLOR_OFF_R, COLOR_OFF_G, COLOR_OFF_B);
    delay(LED_BLINK_DELAY_MS);
  }
}


void setup()
{
  Serial.begin(SERIAL_BAUD);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_FIRE, INPUT);
  pinMode(PIN_NTC, INPUT);

  // задаємо опорну напругу Vref = Vcc
  // АЦП вимірює частку від Vcc (Vout / Vcc)
  // АЦП має 10 біт → 2 ^ 10 = 1024 та -1, оскільки рахуємо від 0
  analogReference(DEFAULT);
}

// Встановлення кольору RGB-світлодіода
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(PIN_RED, redValue);
  analogWrite(PIN_GREEN, greenValue);
  analogWrite(PIN_BLUE, blueValue);
}

float ReadNtcTemperature()
{
  const int   ADC_MAX   = 1023;                 // 10-бітний АЦП Arduino UNO/Nano (2 ^ 10 - 1 значень)
  const float RSER_OHM  = 4700.0;               // 4.7 кОм до GND
  const float R0_OHM    = 10000.0;              // 10 кОм @ 25°C
  const float BETA      = 3950.0;               // коеф. B для MF52AT (уточнити по даташиту)
  const float T0_C      = 25.0;                 // Номінальна температура 25°C
  const float T0_K      = T0_C + KELVIN_OFFSET; // 298.15 K

  int adc = analogRead(PIN_NTC);
  if (adc < 1)
  {
    // захист від ділення на нуль, якщо сенсор від'єднаний
    adc = 1; 
  }

  // шукаємо опір терморезистора
  // Rntc = RSER_OHM * ((ADC_MAX / adc) - 1)
  float Rntc = RSER_OHM * ( (float)ADC_MAX / (float)adc - 1.0 );

  float invT  = (1.0 / T0_K) + (1.0 / BETA) * log(Rntc / R0_OHM);

  // “інверсія”, щоб повернутись від 1 / T до самої температури T
  float tempK = 1.0 / invT;
  float tempC = tempK - KELVIN_OFFSET;
  return tempC;
}

void loop()
{
  isSafe = false;
  isWarning = false;
  isDanger = false;

  int state = digitalRead(PIN_FIRE);
  float temp = ReadNtcTemperature();
  Serial.println(temp);
  Serial.println(state);

  if (state == LOW || temp > TEMP_DANGER_THRESHOLD) {
    isDanger = true;
  }
  else if (temp > TEMP_WARNING_THRESHOLD) {
    isWarning = true;
  }
  else if (temp <= TEMP_WARNING_THRESHOLD) {
    isSafe = true;
  } 

  LED(isDanger, isWarning, isSafe);
}
