#define BUTTON_PIN 9  // connected to boot button
#define ledPin 7 

int lastState = HIGH;  // the previous reading from the boot button input pin GPIO 9
int currentState;     // the current reading from the boot button input pin GPIO 9
void setup() 
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}
void loop() 
{
  currentState = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && currentState == LOW)
  {
    digitalWrite(ledPin, HIGH);   // Turn LED on 
    Serial.println("Boot button is pressed and LED is ON");
  }

  else if (lastState == LOW && currentState == HIGH)
  {
    digitalWrite(ledPin, LOW);   // Turn LED on 
    Serial.println("Boot button is not pressed and LED is OFF");
  }
  lastState = currentState;
  delay(100);
}