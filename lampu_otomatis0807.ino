int relayPin = 7;
int ldrPin = A0;
int ldrValue = 0;
int threshold = 315; 

void setup (){
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  Serial.begin(5000);
}

void loop(){
  ldrValue = analogRead(ldrPin);
  Serial.print(ldrValue);

  if(ldrValue<threshold){
    digitalWrite(relayPin, LOW);
  }

  else{
    digitalWrite(relayPin, HIGH);
  }

  delay(1000);
}