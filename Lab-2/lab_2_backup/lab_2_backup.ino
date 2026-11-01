int PIN_NTC = A0;
int redPin = 5;
int bluePin = 7;
int greenPin = 6;
int isFire = 3;
float temp = 0;


bool isDanger = false;
bool isWarning = false;
bool isSafe = false;

void LED(bool danger, bool warning, bool safe) {
  if (safe) {
    setColor(255, 255, 255);
    delay(500);
    setColor(0, 0, 0);
    delay(500);
  }
  else if (warning) {
    setColor(255, 128, 0);
    delay(500);
    setColor(0, 0, 0);
    delay(500);
  }
  else if (danger) {
    setColor(255, 0, 0);
    delay(500);
    setColor(0, 0, 0);
    delay(500);

  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(isFire, INPUT);
  pinMode(PIN_NTC, INPUT);

  // задаємо опорну напругу Vref​ = Vcc
  // АЦП вимірює частку від Vcc (Vout / Vcc)
  // АЦП має 10 біт → 2 ^ 10 = 1024 та -1, оскільки рахуємо від 0
  analogReference(DEFAULT);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

float ReadNtcTemperature()
{
  const int   ADC_MAX   = 1023;           // 10-бітний АЦП Arduino UNO/Nano (2 ^ 10 - 1 значень)
  const float RSER_OHM  = 4700.0;         // 4.7 кОм до GND
  const float R0_OHM    = 10000.0;        // 10 кОм @ 25°C
  const float BETA      = 3950.0;         // коеф. B для MF52AT (уточнити по даташиту)
  const float T0_K      = 25.0 + 273.15;  // 298.15 K

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

  // “інверсія”, щоб повернутись від 1 / 𝑇 до самої температури 𝑇
  float tempK = 1.0 / invT;
  float tempC = tempK - 273.15;
  return tempC;
}

void loop()
{
isSafe = false;
isWarning = false;
isDanger = false;
 int state = digitalRead(isFire);
 float temp = ReadNtcTemperature();
 Serial.println(temp);
 Serial.println(state);
 if(state == LOW || temp > 30) {
  isDanger = true;
 }
 else if(temp > 25) {
  isWarning = true;
 }
 else if(temp <  30) {
  isSafe = true;
 } 
 LED(isDanger, isWarning, isSafe);
}
