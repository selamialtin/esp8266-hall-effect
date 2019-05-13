int led = 13 ; // LED on arduino
int digitalPin = 12; // linear Hall magnetic sensor digital interface
int analogPin = A0; // linear Hall magnetic sensor analog interface
int digitalVal ; // digital readings
int analogVal; // analog readings
int ledPos;
int alarm;
int timerPos;
void setup ()
{
  pinMode (led, OUTPUT); 
  pinMode (digitalPin, INPUT); 
  //pinMode(analogPin, INPUT); 
  Serial.begin(9600);
}

void timer() {
  if (timerPos++ == 10) {
    Serial.println(alarm);
    if (ledPos == LOW) {
      ledPos = HIGH;
    } else {
      ledPos = LOW;
    }
  
    if (alarm > 80) {
      digitalWrite(led, HIGH); 
    } else {
      digitalWrite(led, ledPos); 
    } 

    alarm = 0;
    timerPos = 0;
  }
}

void loop ()
{
  // Read the digital interface
  digitalVal = digitalRead(digitalPin) ; 
  if (digitalVal == LOW) // When magnetic field is present, Arduino LED is on
  {
    alarm += 10;
  }
  timer();
  
  // Read the analog interface
  //analogVal = analogRead(analogPin);
  //Serial.println(analogVal); // print analog value
  //Serial.println(digitalVal); // print analog value

  delay(50);
}
