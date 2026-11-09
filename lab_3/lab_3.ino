const int PIN_BTN_RESET   = 2;  
const int PIN_TRIG_R      = 3; 

const int PIN_SENSE_Q     = 8;  
const int PIN_SENSE_NQ    = 9; 

const int PIN_LED_INVALID = 12; 

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BTN_RESET, INPUT_PULLUP);

  pinMode(PIN_TRIG_R, OUTPUT);
  digitalWrite(PIN_TRIG_R, LOW);

  pinMode(PIN_SENSE_Q, INPUT);
  pinMode(PIN_SENSE_NQ, INPUT);

  pinMode(PIN_LED_INVALID, OUTPUT);
  digitalWrite(PIN_LED_INVALID, LOW);
}

void loop() {
  if (digitalRead(PIN_BTN_RESET) == LOW) {
    digitalWrite(PIN_TRIG_R, HIGH);
  } else {
    digitalWrite(PIN_TRIG_R, LOW);
  }

  int stateQ  = digitalRead(PIN_SENSE_Q);
  int stateNQ = digitalRead(PIN_SENSE_NQ);

  int invalidState = (stateQ == LOW && stateNQ == LOW) ? 1 : 0;
  digitalWrite(PIN_LED_INVALID, invalidState ? HIGH : LOW);

  Serial.print("Q:");
  Serial.print(stateQ);
  Serial.print(" ");
  Serial.print("not_Q:");
  Serial.print(stateNQ);
  Serial.print(" ");
  Serial.print("Invalid_State:");
  Serial.println(invalidState);

  delay(40);
}