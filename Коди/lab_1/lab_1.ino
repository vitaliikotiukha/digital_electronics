
int inputPin = 4;      
int led1 = 7;          
int led2 = 8;         
int led3 = 9;          

int currentLed = 0;     

void setup() {
  pinMode(inputPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);


  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
}

void loop() {
  int state = digitalRead(inputPin);

  if (state == HIGH) {
    currentLed++; 

    if (currentLed > 3) {
      currentLed = 0;
    }


    if (currentLed == 1) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
    } 
    else if (currentLed == 2) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
    } 
    else if (currentLed == 3) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
    }
    delay(1000);
  }
}
